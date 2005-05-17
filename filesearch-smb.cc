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
#include "globals.h"
//#include "typedefs.h"
#include "e_samba.h"
#include "options.h"
//#include "io.h"
//#include "convert.h"

#include "c_string.h"
#include "c_path.h"
#include "c_ipaddr.h"
#include "c_unsigned.h"
#include "c_signed.h"
#include "c_stream.h"

c_ipaddr    address   ;
c_path      rootpath  ;
c_string    username  ;
c_string    password  ;
c_string    workgroup ;
c_string    selfname  ;
c_unsigned  timeout   ;
c_unsigned  depth     ;
//!!!t_ipc_vec   excludes  ;

SMBCCTX * ctx = NULL;

typedef std::stack< std::pair<c_path,c_unsigned> >  t_pathstack;
t_pathstack pathstack;



void auth_fn (const char *server, const char *share, char *_workgroup, int wgmaxlen, char *_username, int unmaxlen, char *_password, int pwmaxlen)
{
	strncpy(_workgroup, workgroup.c_str(), wgmaxlen-1);
	strncpy(_username ,  username.c_str(), unmaxlen-1);
	strncpy(_password ,  password.c_str(), pwmaxlen-1);
}



/*
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
}*/

int main(int argc, char ** argv, char ** env) {
	int exitcode = 0;
	try
	{
		s_log.fd(1);
		s_debug.fd(2);
		c_stream s_task(0);
		c_stream s_data(3);
		// получение параметров вызова программы и занесение их в переменные
		DEBUG("Getting initial parameters for scanning.");
		timeval timer;
		timer.tv_sec  = options::timeout_params_sec;
		timer.tv_usec = options::timeout_params_usec;
		c_stream::t_map task = s_task.read_map(&timer);
		DEBUG("Reading of parameters finished with status "+io::sstatus2print(status)+". Starting to parse them.");
		if (dynamic_cast<c_ipaddr  *>(task[ipc_code_ipaddr   ])) address   = *dynamic_cast<c_ipaddr  *>(task[ipc_code_ipaddr   ]); else throw e_basic("Can not get ip address.");
		if (dynamic_cast<c_path    *>(task[ipc_code_rootpath ])) rootpath  = *dynamic_cast<c_path    *>(task[ipc_code_rootpath ]); else throw e_basic("Can not get root path.");
		if (dynamic_cast<c_string  *>(task[ipc_code_username ])) username  = *dynamic_cast<c_string  *>(task[ipc_code_username ]); else throw e_basic("Can not get user name.");
		if (dynamic_cast<c_string  *>(task[ipc_code_password ])) password  = *dynamic_cast<c_string  *>(task[ipc_code_password ]); else throw e_basic("Can not get password.");
		if (dynamic_cast<c_string  *>(task[ipc_code_workgroup])) workgroup = *dynamic_cast<c_string  *>(task[ipc_code_workgroup]); else throw e_basic("Can not get self workgroup.");
		if (dynamic_cast<c_string  *>(task[ipc_code_selfname ])) selfname  = *dynamic_cast<c_string  *>(task[ipc_code_selfname ]); else throw e_basic("Can not get self name.");
		if (dynamic_cast<c_unsigned*>(task[ipc_code_timeout  ])) timeout   = *dynamic_cast<c_unsigned*>(task[ipc_code_timeout  ]); else throw e_basic("Can not get timeout.");
		if (dynamic_cast<c_unsigned*>(task[ipc_code_depth    ])) depth     = *dynamic_cast<c_unsigned*>(task[ipc_code_depth    ]); else throw e_basic("Can not get depth.");
		if (username.empty()) username = options::smb_guestusername;
		timeout = timeout * 1000;
		DEBUG("Successfully got the following parameters for scanning:");
		DEBUG("address='"  +address.ascii() +"'"   );
		DEBUG("rootpath='" +rootpath.ascii()+"'"   );
		DEBUG("username='" +username.get()  +"'"   );
		DEBUG("password='" +password.get()  +"'"   );
		DEBUG("workgroup='"+workgroup.get() +"'"   );
		DEBUG("selfname='" +selfname.get()  +"'"   );
		DEBUG("timeout="   +timeout.ascii() +"msec");
		DEBUG("depth="     +depth.ascii()          );
//!!!		DEBUG("Reading list of exclusions for scanning.");
//!!!		excludes = io::readvec(io::fd_task, &timer, &status, options::ipc_terminator, true, true);
//!!!		DEBUG("Successfully got "+convert::si2str(excludes.size())+" exclusions for scanning.");
		DEBUG("Finished reading task.");
		// initializing smbclient
		DEBUG("Creating smbc context.");
		ctx = smbc_new_context();
		if (!ctx)
		{
			throw e_samba("Can not allocate context for smbclient.");
		}
		DEBUG("Created smbc context.");
		ctx->debug = 0;
		ctx->timeout = timeout.get();
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
		ctx->timeout = timeout.get();
		// putting initial path to stack
		t_pathstack::value_type t;
		t.second = 1;
		pathstack.push(t);
		std::string rootpathstr = std::string("smb://") + address.ascii() + (rootpath.empty()?"":"/") + rootpath.ascii();
		//
		DEBUG("Starting recursive scan in '"+rootpathstr+"'.");
		while (!pathstack.empty())
		{
			// getting path from stack
			c_path  currpath = pathstack.top().first;
			c_unsigned level = pathstack.top().second;
			pathstack.pop();
			std::string currpathstr = (currpath.empty()?"":"/") + currpath.ascii();
			std::string openpathstr = rootpathstr + currpathstr;
			//
			DEBUG("Opening path '"+openpathstr+"' for directory listing.");
			SMBCFILE * dir = ctx->opendir(ctx, openpathstr.c_str());
			if (!dir)
			{
				LOG("Could not open path '"+openpathstr+"': "+strerror(errno));
			} else {
				DEBUG("Path '"+openpathstr+"' opened.");
//!!!				s_data.write_map();
//!!!				io::writemap(io::fd_data, NULL, NULL, options::terminator, formate_enter(currpathstr));
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
						bool statok = false;
						DEBUG("It is "+(container?"":"not ")+"container & "+(statable?"":"not ")+"statable.");
						if (statable)
						{
							struct stat st;
							DEBUG("Stating '"+path+"'.");
							int stcode = ctx->stat(ctx, path.c_str(), &st);
							if (stcode == -1)
							{
								DEBUG("Stat failed: "+strerror(errno));
							} else {
								DEBUG("Stat successfull.");
								statok = true;
//!!!								if (container)
//!!!								io::writeblock(io::fd_data, NULL, NULL, options::terminator, formate_dir (currpathstr + "/" + name, st));
//!!!								else
//!!!								io::writeblock(io::fd_data, NULL, NULL, options::terminator, formate_file(currpathstr + "/" + name, st));
							}
						} else {
								statok = true;
//!!!								io::writeblock(io::fd_data, NULL, NULL, options::terminator, formate_resource(currpathstr + "/" + name));
						}
						if (container && statok && level < depth)
						{
							DEBUG("Putting recursible item '"+path+"' for future scanning.");
							t_pathstack::value_type t;
							t.first = currpath; t.first.add(name);
							t.second = level+1;
							pathstack.push(t);
						}
						DEBUG("Work with entry finished.");
					}
				}
//!!!				io::writeblock(io::fd_data, NULL, NULL, options::terminator, formate_leave(currpathstr));
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
