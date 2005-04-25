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
		if (setitimer(ITIMER_REAL, &timer, NULL) != 0)
		{
			throw e_basic("Can not set timer.", errno, strerror(errno)); //!!! e_basic?
		}
	}
}

void timer_signal (int)
{
//!!!	log it thread-safe way
//!!!	cerr << "Timed out. Exiting." << endl;
	_exit(2);
}

int main(int argc, char ** argv, char ** env) {
	int exitcode = 0;
	try
	{
		// создание файловых дескрипторов и потоков
		fdistream cin(4);
		fdostream cout(5);
		// получение параметров вызова программы и занесение их в переменные
		address    = utils::getline(&cin);
		rootpath_s = utils::getline(&cin); rootpath = utils::string2path(rootpath_s);
		username   = utils::getline(&cin); if (username.empty()) username = GUEST;
		password   = utils::getline(&cin);
		workgroup  = utils::getline(&cin);
		selfname   = utils::getline(&cin);
		timeout_s  = utils::getline(&cin); timeout = utils::strtoul(timeout_s) * 1000;
		depth_s    = utils::getline(&cin); depth   = utils::strtoul(depth_s  );
		// initializing timer for forced exiting on timeout
		utils::signal_catch(SIGALRM, timer_signal);
		utils::signal_unblock(SIGALRM);
		timer.it_interval.tv_sec  = timeout / 1000 + 1;
		timer.it_interval.tv_usec = 0;
		timer_touch();
		// initializing smbclient
		ctx = smbc_new_context();
		if (!ctx)
		{
			throw e_samba("Can not allocate context for smbclient.");
		}
		ctx->debug = 0;
		ctx->timeout = timeout;
		ctx->netbios_name = strdup(selfname.c_str());
		ctx->workgroup = strdup(workgroup.c_str());
		ctx->callbacks.auth_fn = auth_fn;
		if (!smbc_init_context(ctx))
		{
			smbc_free_context(ctx, false); ctx = NULL;
			throw e_samba("Can not initialize smbclient.", strerror(errno));
		}
		ctx->timeout = timeout;
		// recursive scan
		pair<c_path,unsigned> t;
		t.second = 1;
		paths.push(t);
		//
		while (!paths.empty())
		{
			timer_touch();
			// getting path from stack
			c_path currpath = paths.top().first;
			unsigned level = paths.top().second;
			paths.pop();
			// creating paths
			string currpathstr; for (c_path::iterator i = currpath.begin(); i != currpath.end(); i++) currpathstr += "/" + *i;
			string rootpathstr; for (c_path::iterator i = rootpath.begin(); i != rootpath.end(); i++) rootpathstr += "/" + *i;
			string openpathstr = string("smb://") + address + rootpathstr + currpathstr;
			//
			SMBCFILE * dir = ctx->opendir(ctx, openpathstr.c_str());
			if (!dir)
			{
				//!!! make it pretty. this is not an exception! we could & should scan more!
				cerr << "    " << "Could not open [" << openpathstr << "] (" << setw(0) << dec << errno << "):" << strerror(errno) << endl;
			} else {
				smbc_dirent * dirent;
				while ((dirent = ctx->readdir(ctx, dir)) != NULL)
				{
					timer_touch();
					string name = dirent->name;
					if ((name != "..") && (name != "."))
					{
						string path = openpathstr + "/" + name;
						bool container = (dirent->smbc_type == SMBC_DIR       ) ||
								 (dirent->smbc_type == SMBC_FILE_SHARE) ||
								 (dirent->smbc_type == SMBC_SERVER    ) ||
								 (dirent->smbc_type == SMBC_WORKGROUP );
						bool statable =  (dirent->smbc_type == SMBC_DIR       ) ||
								 (dirent->smbc_type == SMBC_FILE      );
						struct stat st; memset(&st, 0, sizeof(st));
						int stcode = statable ? ctx->stat(ctx, path.c_str(), &st) : 0;
						if (stcode != -1)
						{
							cout
								<< currpathstr + "/" + name << endl
								<< (container?"1":"0")      << endl
								<< st.st_size               << endl
								<< st.st_ctime              << endl
								<< st.st_mtime              << endl
								<< endl;
						}
						if (container && level < depth)
						{
							pair<c_path,unsigned> t;
							t.first = currpath; t.first.push_back(name);
							t.second = level+1;
							paths.push(t);
						}
					}
				}
				ctx->closedir(ctx, dir);
				cout << "#"
					<< currpathstr << endl
					<< endl;
			}
		}
		// freeing smbclient
		smbc_free_context(ctx, true); ctx = NULL;
		//
		utils::signal_block(SIGALRM);
		utils::signal_uncatch(SIGALRM);
	}
	catch (e_basic &e)
	{
		cerr << "Exception: " <<  e.what() << endl;
		exitcode = -2;
	}
	catch (...)
	{
		cerr << "Unknown exception without message." << endl;
		exitcode = -1;
	}
	if (ctx) { smbc_free_context(ctx, false); ctx = NULL; }
	return exitcode;
}
