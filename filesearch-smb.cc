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
#include <libsmbclient.h>

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

void auth_fn (const char *server, const char *share, char *_workgroup, int wgmaxlen, char *_username, int unmaxlen, char *_password, int pwmaxlen)
{
	strncpy(_workgroup, workgroup.c_str(), wgmaxlen-1);
	strncpy(_username ,  username.c_str(), unmaxlen-1);
	strncpy(_password ,  password.c_str(), pwmaxlen-1);
}


void recursive (c_path currpath, unsigned int level)
{
	// creating paths
	string currpathstr; for (c_path::iterator i = currpath.begin(); i != currpath.end(); i++) currpathstr += "/" + *i;
	string rootpathstr; for (c_path::iterator i = rootpath.begin(); i != rootpath.end(); i++) rootpathstr += "/" + *i;
	string openpathstr = string("smb://") + address + rootpathstr + currpathstr;

	// reading dir content and closing it
	map<string,unsigned int> dirents;
	if (address == "10.0.1.8") cerr << "ReadDir'ing '" << openpathstr << "' timeout=" << ctx->timeout << endl;
	SMBCFILE * dir = ctx->opendir(ctx, openpathstr.c_str());
	if (!dir)
	{
		//!!! make it pretty. this is not an exception! we could & should scan more!
		cerr << "    " << "Could not open [" << openpathstr << "] (" << setw(0) << dec << errno << "):" << strerror(errno) << endl;
	} else {
		if (address == "10.0.1.8") cerr << "Dir '" << openpathstr << "' ok" << endl;
		smbc_dirent * dirent;
		while ((dirent = ctx->readdir(ctx, dir)) != NULL)
		{
			if (address == "10.0.1.8") cerr << "Got '" << dirent->name << "' in '" << openpathstr << "'" << endl;
			dirents[dirent->name] = dirent->smbc_type;
		}
		ctx->closedir(ctx, dir);
		if (address == "10.0.1.8") cerr << "Closed dir '" << openpathstr << "'" << endl;
	}
	if (address == "10.0.1.8") cerr << "CloseDir'ing '" << openpathstr << "'" << endl;

	// iterating through directory entries
	for (map<string,unsigned int>::iterator dirent = dirents.begin(); depth && dirent != dirents.end(); dirent++)
	{
		string name = dirent->first;
		if ((name != "..") && (name != "."))
		{
			string path = openpathstr + "/" + name;
			bool container = (dirent->second == SMBC_DIR       ) ||
					 (dirent->second == SMBC_FILE_SHARE) ||
					 (dirent->second == SMBC_SERVER    ) ||
					 (dirent->second == SMBC_WORKGROUP );
			bool statable =  (dirent->second == SMBC_DIR       ) ||
					 (dirent->second == SMBC_FILE      );
			struct stat st; memset(&st, 0, sizeof(st));
			if (address == "10.0.1.8") cerr << "Trying to stat '" << path << "' on " << address << endl;
			int stcode = statable ? ctx->stat(ctx, path.c_str(), &st) : 0;
			if (address == "10.0.1.8") cerr << "Finished stating '" << path << "' on " << address << endl;
			if (stcode != -1)
			{
				cout
					<< currpathstr + "/" + name << endl
					<< (container?"1":"0")      << endl
					<< st.st_size               << endl
					<< st.st_ctime              << endl
					<< st.st_mtime              << endl
					<< endl;
				if (address == "10.0.1.8" )
				{cerr
					<< currpathstr + "/" + name << endl
					<< (container?"1":"0")      << endl
					<< st.st_size               << endl
					<< st.st_ctime              << endl
					<< st.st_mtime              << endl
					<< endl;
				}
			}
			if (container && level < depth)
			{
				if (address == "10.0.1.8") cerr << "Recusring to '" << currpathstr + "/" + name << "'" << endl;
				c_path subpath = currpath; subpath.push_back(name);
				recursive(subpath, level+1);
			}
		}
	}
}


int main(int argc, char ** argv, char ** env) {
	int exitcode = 0;
	try
	{
		// получение параметров вызова программы и занесение их в переменные
		address    = utils::getline(&cin);
		rootpath_s = utils::getline(&cin); rootpath = utils::string2path(rootpath_s);
		username   = utils::getline(&cin); if (username.empty()) username = GUEST;
		password   = utils::getline(&cin);
		workgroup  = utils::getline(&cin);
		selfname   = utils::getline(&cin);
		timeout_s  = utils::getline(&cin); timeout = utils::strtoul(timeout_s);
		depth_s    = utils::getline(&cin); depth   = utils::strtoul(depth_s  );
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
			smbc_free_context(ctx, true);
			throw e_samba("Can not initialize smbclient.", strerror(errno));
		}
		// recursive scan
		c_path emptypath;
		recursive(emptypath, 1);
		// freeing smbclient
		smbc_free_context(ctx, false); ctx = NULL;
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
