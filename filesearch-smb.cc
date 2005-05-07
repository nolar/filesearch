/*
 * Сканер ресурсов обслуживаемых компьютеров.
 *
 * Формат входных данных (stdin):
 *
 * Формат выходных данных (stdout):
 *
 * (a) Sergei Vasilyev aka nolar 2005
 */

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stack>
#include <libsmbclient.h>
#include <sys/time.h>
#include "config.h"
#include "typedefs.h"		    
#include "e_samba.h"
#include "options.h"
#include "io.h"
#include "convert.h"

std::string address_s;
std::string rootpath_s;
std::string username;
std::string password;
std::string workgroup;
std::string selfname;
std::string timeout_s;
std::string depth_s;
t_ipaddr address;
t_path rootpath;
t_timeout timeout;
t_depth   depth;

SMBCCTX * ctx = NULL;

typedef std::stack< std::pair<t_path,t_depth> >  t_pathstack;
t_pathstack pathstack;



void auth_fn (const char *server, const char *share, char *_workgroup, int wgmaxlen, char *_username, int unmaxlen, char *_password, int pwmaxlen)
{
	strncpy(_workgroup, workgroup.c_str(), wgmaxlen-1);
	strncpy(_username ,  username.c_str(), unmaxlen-1);
	strncpy(_password ,  password.c_str(), pwmaxlen-1);
}



std::vector<std::string> formate_resource (std::string path)
{
	std::vector<std::string> result;
	result.push_back(options::action_code_for_resource);
	result.push_back(path.empty()?"/":path);
	return result;
}

std::vector<std::string> formate_dir (std::string path, struct stat st)
{
	std::vector<std::string> result;
	result.push_back(options::action_code_for_dir);
	result.push_back(path.empty()?"/":path);
	result.push_back(convert::time2system(st.st_ctime));
	result.push_back(convert::time2system(st.st_mtime));
	return result;
}

std::vector<std::string> formate_file (std::string path, struct stat st)
{
	std::vector<std::string> result;
	result.push_back(options::action_code_for_file);
	result.push_back(path.empty()?"/":path);
	result.push_back(convert::size2system(st.st_size ));
	result.push_back(convert::time2system(st.st_ctime));
	result.push_back(convert::time2system(st.st_mtime));
	return result;
}

std::vector<std::string> formate_enter (std::string path)
{
	std::vector<std::string> result;
	result.push_back(options::action_code_for_enter);
	result.push_back(path.empty()?"/":path);
	return result;
}

std::vector<std::string> formate_leave (std::string path)
{
	std::vector<std::string> result;
	result.push_back(options::action_code_for_leave);
	result.push_back(path.empty()?"/":path);
	return result;
}

int main(int argc, char ** argv, char ** env) {
	int exitcode = 0;
	try
	{
		// получение параметров вызова программы и занесение их в переменные
		DEBUG("Getting initial parameters for scanning.");
		t_stream_status status;
		timeval timer_params;
		timer_params.tv_sec  = options::timeout_params_sec;
		timer_params.tv_usec = options::timeout_params_usec;
		std::vector<std::string> data, params = io::readblock(io::fd_task, &timer_params, &status, options::terminator, true, true, 8, 8);
		if (params.size() < 8) throw e_io("Can not read required number of parameters (8).");
		DEBUG("Successfully got the following parameters for scanning:");
		address_s  = params[0]; address = convert::str2ipaddr(address_s);
		rootpath_s = params[1]; rootpath = convert::str2path(rootpath_s);
		username   = params[2]; if (username.empty()) username = options::smb_guestusername;
		password   = params[3];
		workgroup  = params[4];
		selfname   = params[5];
		timeout_s  = params[6]; timeout = convert::str2timeout(timeout_s) * 1000;
		depth_s    = params[7]; depth   = convert::str2depth  (depth_s  );
		DEBUG("address='"  +convert::ipaddr2print(address) +"'"   );
		DEBUG("rootpath='" +convert::path2print(rootpath)  +"'"   );
		DEBUG("username='" +username                       +"'"   );
		DEBUG("password='" +password                       +"'"   );
		DEBUG("workgroup='"+workgroup                      +"'"   );
		DEBUG("selfname='" +selfname                       +"'"   );
		DEBUG("timeout="   +convert::timeout2print(timeout)+"msec");
		DEBUG("depth="     +convert::depth2print(depth)           );
		DEBUG("End of parameters list.");
		// initializing smbclient
		DEBUG("Creating smbc context.");
		ctx = smbc_new_context();
		if (!ctx)
		{
			throw e_samba("Can not allocate context for smbclient.");
		}
		DEBUG("Created smbc context.");
		ctx->debug = 0;
		ctx->timeout = timeout;
		ctx->netbios_name = strdup(selfname.c_str());
		ctx->workgroup = strdup(workgroup.c_str());
		ctx->callbacks.auth_fn = auth_fn;
		DEBUG("Initializing smbc context.");
		if (!smbc_init_context(ctx))
		{
			smbc_free_context(ctx, false); ctx = NULL;
			throw e_samba("Can not initialize smbclient.", strerror(errno));
		}
		DEBUG("Initialized smbc context.");
		ctx->timeout = timeout;
		// putting initial path to stack
		t_pathstack::value_type t;
		t.second = 1;
		pathstack.push(t);
		std::string rootpathstr = std::string("smb://") + convert::ipaddr2print(address) + (rootpath.empty()?"":"/") + convert::path2system(rootpath);
		//
		DEBUG("Starting recursive scan in '"+rootpathstr+"'.");
		while (!pathstack.empty())
		{
			// getting path from stack
			t_path currpath = pathstack.top().first;
			t_depth level = pathstack.top().second;
			pathstack.pop();
			std::string currpathstr = (currpath.empty()?"":"/") + convert::path2system(currpath);
			std::string openpathstr = rootpathstr + currpathstr;
			//
			DEBUG("Opening path '"+openpathstr+"' for directory listing.");
			SMBCFILE * dir = ctx->opendir(ctx, openpathstr.c_str());
			if (!dir)
			{
				LOG("Could not open path '"+openpathstr+"': "+strerror(errno));
			} else {
				DEBUG("Path '"+openpathstr+"' opened.");
				io::writeblock(io::fd_data, NULL, NULL, options::terminator, formate_enter(currpathstr));
				smbc_dirent * dirent;
				DEBUG("Listing its entries.");
				while (level <= depth && (dirent = ctx->readdir(ctx, dir)) != NULL)
				{
					std::string name = dirent->name;
					if ((name != "..") && (name != "."))
					{
						DEBUG("Got entry '"+name+"'.");
						std::string path = openpathstr + "/" + name;
						bool container = (dirent->smbc_type == SMBC_DIR       ) ||
								 (dirent->smbc_type == SMBC_FILE_SHARE) ||
								 (dirent->smbc_type == SMBC_SERVER    ) ||
								 (dirent->smbc_type == SMBC_WORKGROUP );
						bool statable =  (dirent->smbc_type == SMBC_DIR       ) ||
								 (dirent->smbc_type == SMBC_FILE      );
						DEBUG("It is "+(container?"":"not ")+"container & "+(statable?"":"not ")+"statable.");
						if (statable)
						{
							struct stat st;
							DEBUG("Stating '"+path+"'.");
							int stcode = ctx->stat(ctx, path.c_str(), &st);
							if (stcode == -1)
							{
								DEBUG("Stat failed: "+strerror(errno));
								memset(&st, 0, sizeof(st));
							} else {
								DEBUG("Stat successfull.");
							}
							if (container)
							io::writeblock(io::fd_data, NULL, NULL, options::terminator, formate_dir (currpathstr + "/" + name, st));
							else
							io::writeblock(io::fd_data, NULL, NULL, options::terminator, formate_file(currpathstr + "/" + name, st));
						} else {
							io::writeblock(io::fd_data, NULL, NULL, options::terminator, formate_resource(currpathstr + "/" + name));
						}
						if (container && level < depth)
						{
							DEBUG("Putting recursible item '"+path+"' for future scanning.");
							t_pathstack::value_type t;
							t.first = currpath; t.first.push_back(name);
							t.second = level+1;
							pathstack.push(t);
						}
						DEBUG("Work with entry finished.");
					}
				}
				io::writeblock(io::fd_data, NULL, NULL, options::terminator, formate_leave(currpathstr));
				DEBUG("Closing dir '"+openpathstr+"'.");
				ctx->closedir(ctx, dir);
				DEBUG("Closed.");
			}
		}
		// freeing smbclient
		DEBUG("Freeing smbc context after work.");
		smbc_free_context(ctx, false); ctx = NULL;
		DEBUG("Freed.");
	}
	catch (e_basic &e)
	{
		LOG("Exception: "+e.what());
		exitcode = 1;
	}
	catch (exception &e)
	{
		LOG("Exception: "+e.what());
		exitcode = 1;
	}
	catch (...)
	{
		LOG("Exception with no type and message.");
		exitcode = 1;
	}
	if (ctx) { smbc_free_context(ctx, false); ctx = NULL; }
	DEBUG("Bye.");
	return exitcode;
}
