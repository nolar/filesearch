/*
 * Сканер ресурсов обслуживаемых компьютеров.
 *
 * Формат входных данных (stdin):
 *
 * Формат выходных данных (stdout):
 *
 * (a) Sergei Vasilyev aka nolar 2005
 */

#include <libsmbclient.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <exception>
#include <sys/time.h>
#include <errno.h>

#include "config.h"
#include "globals.h"
#include "c_stream.h"
#include "e_basic.h"
#include "e_samba.h"

#include "c_signed.h"
#include "c_unsigned.h"
#include "c_string.h"
#include "c_path.h"
#include "c_ipaddr.h"
#include "c_stamp.h"

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

typedef std::queue< std::pair<c_path,c_unsigned> >  t_pathqueue;
t_pathqueue pathqueue;



void auth_fn (const char *server, const char *share, char *_workgroup, int wgmaxlen, char *_username, int unmaxlen, char *_password, int pwmaxlen)
{
	strncpy(_workgroup, workgroup.c_str(), wgmaxlen-1);
	strncpy(_username ,  username.c_str(), unmaxlen-1);
	strncpy(_password ,  password.c_str(), pwmaxlen-1);
}




c_stream::t_map make_resource (const c_path & path)
{
	c_stream::t_map result;
	result[ipc_data_action] = const_cast<c_object*>(dynamic_cast<const c_object*>(&action_resource));
	result[ipc_data_path  ] = const_cast<c_object*>(dynamic_cast<const c_object*>(&path));
	return result;
}

c_stream::t_map make_dir (const c_path & path, const c_stamp & ctime, const c_stamp & mtime)
{
	c_stream::t_map result;
	result[ipc_data_action] = const_cast<c_object*>(dynamic_cast<const c_object*>(&action_dir));
	result[ipc_data_path  ] = const_cast<c_object*>(dynamic_cast<const c_object*>(&path));
	result[ipc_data_ctime ] = const_cast<c_object*>(dynamic_cast<const c_object*>(&ctime));
	result[ipc_data_mtime ] = const_cast<c_object*>(dynamic_cast<const c_object*>(&mtime));
	return result;
}

c_stream::t_map make_file (const c_path & path, const c_unsigned & size, const c_stamp & ctime, const c_stamp & mtime)
{
	c_stream::t_map result;
	result[ipc_data_action] = const_cast<c_object*>(dynamic_cast<const c_object*>(&action_file));
	result[ipc_data_path  ] = const_cast<c_object*>(dynamic_cast<const c_object*>(&path));
	result[ipc_data_size  ] = const_cast<c_object*>(dynamic_cast<const c_object*>(&size));
	result[ipc_data_ctime ] = const_cast<c_object*>(dynamic_cast<const c_object*>(&ctime));
	result[ipc_data_mtime ] = const_cast<c_object*>(dynamic_cast<const c_object*>(&mtime));
	return result;
}

c_stream::t_map make_enter (const c_path & path)
{
	c_stream::t_map result;
	result[ipc_data_action] = const_cast<c_object*>(dynamic_cast<const c_object*>(&action_enter));
	result[ipc_data_path  ] = const_cast<c_object*>(dynamic_cast<const c_object*>(&path));
	return result;
}

c_stream::t_map make_leave (const c_path & path)
{
	c_stream::t_map result;
	result[ipc_data_action] = const_cast<c_object*>(dynamic_cast<const c_object*>(&action_leave));
	result[ipc_data_path  ] = const_cast<c_object*>(dynamic_cast<const c_object*>(&path));
	return result;
}

int main(int argc, char ** argv, char ** env) {
	int exitcode = 0;
	try
	{
		s_log.fd(1);
		s_debug.fd(2);
		c_stream s_task(0); s_task.set_min_timeout(default_timeout_task__sec, default_timeout_task__usec);
		c_stream s_data(3); s_data.set_min_timeout(1); // should be options
		// получение параметров вызова программы и занесение их в переменные
		DEBUG("Getting initial parameters for scanning.");
		c_stream::t_map task = s_task.read_map(NULL);
		DEBUG("Reading of parameters finished with status "+s_task.status_text()+". Starting to parse them.");
		if (dynamic_cast<c_ipaddr  *>(task[ipc_task_ipaddr   ])) address   = *dynamic_cast<c_ipaddr  *>(task[ipc_task_ipaddr   ]); else throw e_basic("Can not get ip address.");
		if (dynamic_cast<c_path    *>(task[ipc_task_rootpath ])) rootpath  = *dynamic_cast<c_path    *>(task[ipc_task_rootpath ]); else throw e_basic("Can not get root path.");
		if (dynamic_cast<c_string  *>(task[ipc_task_username ])) username  = *dynamic_cast<c_string  *>(task[ipc_task_username ]); else throw e_basic("Can not get user name.");
		if (dynamic_cast<c_string  *>(task[ipc_task_password ])) password  = *dynamic_cast<c_string  *>(task[ipc_task_password ]); else throw e_basic("Can not get password.");
		if (dynamic_cast<c_string  *>(task[ipc_task_workgroup])) workgroup = *dynamic_cast<c_string  *>(task[ipc_task_workgroup]); else throw e_basic("Can not get self workgroup.");
		if (dynamic_cast<c_string  *>(task[ipc_task_selfname ])) selfname  = *dynamic_cast<c_string  *>(task[ipc_task_selfname ]); else throw e_basic("Can not get self name.");
		if (dynamic_cast<c_unsigned*>(task[ipc_task_timeout  ])) timeout   = *dynamic_cast<c_unsigned*>(task[ipc_task_timeout  ]); else throw e_basic("Can not get timeout.");
		if (dynamic_cast<c_unsigned*>(task[ipc_task_depth    ])) depth     = *dynamic_cast<c_unsigned*>(task[ipc_task_depth    ]); else throw e_basic("Can not get depth.");
		if (username.empty()) username = default_username_smb;
		timeout = timeout * 1000;
		DEBUG("Successfully got the following parameters for scanning:");
		DEBUG("address='"  +address.ascii() +"'"   );
		DEBUG("rootpath='" +rootpath.ascii(true, true)+"'"   );
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
		// putting initial path to queue
		t_pathqueue::value_type t;
		t.second = 1;
		pathqueue.push(t);
//!!!		std::string rootpathstr = std::string("smb://") + address.ascii() + (rootpath.empty()?"":"/") + rootpath.ascii(true);
		std::string prefix = std::string("smb://") + address.ascii() + rootpath.ascii(true, false);
		//
		DEBUG("Starting recursive scan in '"+prefix+"'.");
		while (!pathqueue.empty())
		{
			// getting path from queue
			c_path  currpath = pathqueue.front().first;
			c_unsigned level = pathqueue.front().second;
			pathqueue.pop();
//!!!			std::string currpathstr = (currpath.empty()?"":"/") + currpath.ascii(true);
//!!!			std::string openpathstr = rootpathstr + currpathstr;
			//
			DEBUG("Opening path '"+prefix+currpath.ascii(true, false)+"' for directory listing.");
			SMBCFILE * dir = ctx->opendir(ctx, (prefix+currpath.ascii(true, false)).c_str());
			if (!dir)
			{
				LOG("Could not open path '"+prefix+currpath.ascii(true, false)+"': "+strerror(errno));
			} else {
				DEBUG("Path '"+prefix+currpath.ascii(true, false)+"' opened.");
				s_data.write_map(make_enter(currpath), NULL);
				smbc_dirent * dirent;
				DEBUG("Listing its entries.");
				while (level <= depth && (dirent = ctx->readdir(ctx, dir)) != NULL)
				{
					std::string name = dirent->name;
					if ((name != "..") && (name != "."))
					{
						DEBUG("Got entry '"+name+"'.");
						c_path path = currpath; path.add(name);
//!!!						std::string path = openpathstr + "/" + name;
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
							DEBUG("Stating '"+prefix+path.ascii(true, false)+"'.");
							int stcode = ctx->stat(ctx, (prefix+path.ascii(true, false)).c_str(), &st);
							if (stcode == -1)
							{
								DEBUG("Stat failed: "+strerror(errno));
							} else {
								DEBUG("Stat successfull.");
								c_stamp ctime(st.st_ctime);
								c_stamp mtime(st.st_mtime);
								c_unsigned size(st.st_size);
								statok = true;
								if (container)
								s_data.write_map(make_dir(path, ctime, mtime));
								else
								s_data.write_map(make_file(path, size, ctime, mtime));
							}
						} else {
								statok = true;
								s_data.write_map(make_resource(path));
						}
						if (container && statok && level < depth)
						{
							DEBUG("Putting recursible item '"+prefix+path.ascii(true, false)+"' for future scanning.");
							t_pathqueue::value_type t;
							t.first = path;
							t.second = level+1;
							pathqueue.push(t);
						}
						DEBUG("Work with entry finished.");
					}
				}
				s_data.write_map(make_leave(currpath), NULL);
				DEBUG("Closing dir '"+prefix+currpath.ascii(true, false)+"'.");
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
	catch (std::exception &e)
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
