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
#include <map>
#include <vector>
#include <stack>
#include <libsmbclient.h>
#include <sys/time.h>
		    
#include "e_samba.h"
#include "utils.h"
#include "options.h"

#define GUEST "guest"

using namespace std;

string address;
string rootpath_s;
string username;
string password;
string workgroup;
string selfname;
string timeout_s;
string depth_s;
c_path rootpath;
unsigned int timeout;
unsigned int depth;

SMBCCTX * ctx = NULL;
stack< pair<c_path,unsigned> > paths;
itimerval timer;

void auth_fn (const char *server, const char *share, char *_workgroup, int wgmaxlen, char *_username, int unmaxlen, char *_password, int pwmaxlen)
{
	strncpy(_workgroup, workgroup.c_str(), wgmaxlen-1);
	strncpy(_username ,  username.c_str(), unmaxlen-1);
	strncpy(_password ,  password.c_str(), pwmaxlen-1);
}

void timer_touch ()
{
	timer.it_value = timer.it_interval;
	if (timer.it_interval.tv_sec || timer.it_interval.tv_usec)
	{
//		if (setitimer(ITIMER_REAL, &timer, NULL) != 0)
//		{
//			throw e_basic("Can not set timer.", errno, strerror(errno)); //!!! e_basic?
//		}
	}
}

void timer_signal (int)
{
//!!!	log it thread-safe way
//!!!	cerr << "Timed out. Exiting." << endl;

//	_exit(2);
}


vector<string> formate_resource (string path)
{
	vector<string> result;
	result.push_back(options::action_code_for_resource);
	result.push_back(path.empty()?"/":path);
	return result;
}

vector<string> formate_dir (string path, struct stat st)
{
	vector<string> result;
	result.push_back(options::action_code_for_dir);
	result.push_back(path.empty()?"/":path);
	result.push_back(utils::ultostr(st.st_ctime));
	result.push_back(utils::ultostr(st.st_mtime));
	return result;
}

vector<string> formate_file (string path, struct stat st)
{
	vector<string> result;
	result.push_back(options::action_code_for_file);
	result.push_back(path.empty()?"/":path);
	result.push_back(utils::ultostr(st.st_size));
	result.push_back(utils::ultostr(st.st_ctime));
	result.push_back(utils::ultostr(st.st_mtime));
	return result;
}

vector<string> formate_enter (string path)
{
	vector<string> result;
	result.push_back(options::action_code_for_enter);
	result.push_back(path.empty()?"/":path);
	return result;
}

vector<string> formate_leave (string path)
{
	vector<string> result;
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
		timer_params.tv_sec  = options::timeout_params_sec ;
		timer_params.tv_usec = options::timeout_params_usec;
		vector<string> data, params = utils::readblock(options::fd_task, &timer_params, &status, options::terminator, true, true, 8, 8);
		if (params.size() < 8) throw e_basic("Can not read required parameters.");
		DEBUG("Successfully got the following parameters for scanning:");
		address    = params[0];
		rootpath_s = params[1]; rootpath = utils::string2path(rootpath_s);
		username   = params[2]; if (username.empty()) username = GUEST;
		password   = params[3];
		workgroup  = params[4];
		selfname   = params[5];
		timeout_s  = params[6]; timeout = utils::strtoul(timeout_s) * 1000;
		depth_s    = params[7]; depth   = utils::strtoul(depth_s  );
		DEBUG("address='"+address+"'");
		DEBUG("rootpath='"+utils::path2string(rootpath)+"'");
		DEBUG("username='"+username+"'");
		DEBUG("password='"+password+"'");
		DEBUG("workgroup='"+workgroup+"'");
		DEBUG("selfname='"+selfname+"'");
		DEBUG("timeout="+utils::ultostr(timeout)+"msec");
		DEBUG("depth="+utils::ultostr(depth));
		DEBUG("End of parameters list.");
		// initializing timer for forced exiting on timeout
//!!!		utils::signal_catch(SIGALRM, timer_signal);
//!!!		utils::signal_unblock(SIGALRM);
		timer.it_interval.tv_sec  = timeout / 1000 + 1;
		timer.it_interval.tv_usec = 0;
		timer_touch();
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
		// recursive scan
		pair<c_path,unsigned> t;
		t.second = 1;
		paths.push(t);
		string rootpathstr = string("smb://") + address;
		for (c_path::iterator i = rootpath.begin(); i != rootpath.end(); i++)
			rootpathstr += "/" + *i;
		//
		DEBUG("Starting recursive scan in '"+rootpathstr+"'.");
		while (!paths.empty())
		{
			timer_touch();
			// getting path from stack
			c_path currpath = paths.top().first;
			unsigned level = paths.top().second;
			paths.pop();
			string currpathstr; for (c_path::iterator i = currpath.begin(); i != currpath.end(); i++) currpathstr += "/" + *i;
			string openpathstr = rootpathstr + currpathstr;
			//
			DEBUG("Opening path '"+openpathstr+"' for directory listing.");
			SMBCFILE * dir = ctx->opendir(ctx, openpathstr.c_str());
			if (!dir)
			{
				LOG("Could not open path '"+openpathstr+"': code "+utils::ultostr(errno)+" ("+strerror(errno)+")");
			} else {
				DEBUG("Path '"+openpathstr+"' opened.");
				utils::writeblock(options::fd_data, NULL, NULL, options::terminator, formate_enter(currpathstr));
				smbc_dirent * dirent;
				DEBUG("Listing its entries.");
				while ((dirent = ctx->readdir(ctx, dir)) != NULL)
				{
					timer_touch();
					string name = dirent->name;
					if ((name != "..") && (name != "."))
					{
						DEBUG("Got entry '"+name+"'.");
						string path = openpathstr + "/" + name;
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
								DEBUG("Stat failed with code "+utils::ultostr(stcode)+": "+strerror(errno));
								memset(&st, 0, sizeof(st));
							} else {
								DEBUG("Stat successfull.");
							}
							if (container)
							utils::writeblock(options::fd_data, NULL, NULL, options::terminator, formate_dir (currpathstr + "/" + name, st));
							else
							utils::writeblock(options::fd_data, NULL, NULL, options::terminator, formate_file(currpathstr + "/" + name, st));
						} else {
							utils::writeblock(options::fd_data, NULL, NULL, options::terminator, formate_resource(currpathstr + "/" + name));
						}
						if (container && ((depth <= 0) || (level < depth)))
						{
							DEBUG("Putting recursible item '"+openpathstr+"/"+name+"' for future scanning.");
							pair<c_path,unsigned> t;
							t.first = currpath; t.first.push_back(name);
							t.second = level+1;
							paths.push(t);
						}
						DEBUG("Work with entry finished.");
					}
				}
				utils::writeblock(options::fd_data, NULL, NULL, options::terminator, formate_leave(currpathstr));
				DEBUG("Closing dir '"+openpathstr+"'.");
				ctx->closedir(ctx, dir);
				DEBUG("Closed.");
			}
		}
		// freeing smbclient
		DEBUG("Freeing smbc context after work.");
		smbc_free_context(ctx, false); ctx = NULL;
		DEBUG("Freed.");
		//
		utils::signal_block(SIGALRM);
		utils::signal_uncatch(SIGALRM);
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
