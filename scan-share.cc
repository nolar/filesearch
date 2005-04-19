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
string share;
string username;
string password;
string workgroup;
string selfname;
string timeout_s;
string depth_s;
unsigned int timeout;
unsigned int depth;
bool statfiles;

SMBCCTX * ctx = NULL;

void auth_fn (const char *server, const char *share, char *_workgroup, int wgmaxlen, char *_username, int unmaxlen, char *_password, int pwmaxlen)
{
	strncpy(_workgroup, workgroup.c_str(), wgmaxlen-1);
	strncpy(_username ,  username.c_str(), unmaxlen-1);
	strncpy(_password ,  password.c_str(), pwmaxlen-1);
}


void recursive (c_path root, unsigned int level)
{
	// creating paths
	string relative;
	for (c_path::iterator i = root.begin(); i != root.end(); i++) relative += "/" + *i;
	string rootstr = string() + "smb://" + address + (share.empty()?"":"/") + share + relative;

	// reading dir content and closing it
	map<string,unsigned int> dirents;
	SMBCFILE * dir = ctx->opendir(ctx, rootstr.c_str());
	if (!dir)
	{
		cerr << "    " << "Could not open [" << rootstr << "] (" << setw(0) << dec << errno << "):" << strerror(errno) << endl;
	} else {
		smbc_dirent * dirent;
		while ((dirent = ctx->readdir(ctx, dir)) != NULL)
		{
			dirents[dirent->name] = dirent->smbc_type;
		}
		ctx->closedir(ctx, dir);
	}

	// iterating through directory entries
	for (map<string,unsigned int>::iterator dirent = dirents.begin(); dirent != dirents.end(); dirent++)
	{
		string name = dirent->first;
		if ((name != "..") && (name != "."))
		{
			string path = rootstr + "/" + name;
			bool container = (dirent->second == SMBC_DIR       ) ||
					 (dirent->second == SMBC_FILE_SHARE) ||
					 (dirent->second == SMBC_SERVER    ) ||
					 (dirent->second == SMBC_WORKGROUP );
			struct stat st; memset(&st, 0, sizeof(st));
			int stcode = statfiles ? ctx->stat(ctx, path.c_str(), &st) : 0;
			if (stcode != -1)
			{
				cout
					<< relative + "/" + name << endl
					<< (container?"1":"0") << endl
					<< st.st_size          << endl
					<< st.st_ctime         << endl
					<< st.st_mtime         << endl
					<< endl;
			}
			if (container && level < depth)
			{
				c_path subpath = root; subpath.push_back(name);
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
		char buffer[1025];
		cin.getline(buffer, 1024); address   = buffer;
		cin.getline(buffer, 1024); share     = buffer;
		cin.getline(buffer, 1024); username  = buffer;
		cin.getline(buffer, 1024); password  = buffer;
		cin.getline(buffer, 1024); workgroup = buffer;
		cin.getline(buffer, 1024); selfname  = buffer;
		cin.getline(buffer, 1024); timeout_s = buffer;
		cin.getline(buffer, 1024); depth_s   = buffer;
		// parsing and correcting
		timeout = utils::strtoul(timeout_s);
		depth   = utils::strtoul(depth_s  );
		statfiles = !share.empty();
		if (share.empty()) depth = 1;
		if (username.empty()) username = GUEST;
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
	catch (e_iconv &e)
	{
		cerr << "Exception: " <<  e.what() << endl;
		exitcode = -2;
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
