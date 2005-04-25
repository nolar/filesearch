#include "e_database.h"
#include "database.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "utils.h"
#include "thread.h"
#include "thread_smb.h"
#include "options.h"

extern c_database * database;

//
vector< pair<string,t_id> > thread_smb__shares;

// when scanning host for a list of shares, doing it when first share was found
void thread_smb__share_topaction (c_request request)
{
	database->status_renew(request);
}

// when scanning host for a list of shares, doing it for each found share
void thread_smb__share_subaction (c_request request, c_fileinfo fileinfo)
{
	pair<string,t_id> item;
	item.first  = fileinfo.name();
	item.second = database->report_share(request, fileinfo.name());
	thread_smb__shares.push_back(item);
}

// 
void thread_smb__share_flushaction (c_request request, c_path)
{
	database->flush_shares(request);
}



// when scanning share for a list of files, doing it when first file was found
void thread_smb__file_topaction (c_request request)
{
	database->status_renew(request);
}

// when scanning share for a list of files, doing it for each found file
void thread_smb__file_subaction (c_request request, c_fileinfo fileinfo)
{
//!!!	string s; c_path p = fileinfo.path();
//!!!	for (c_path::const_iterator i = p.begin(); i != p.end(); i++) s += "/" + *i;
//!!!	cerr << (fileinfo.container()?"Dir  '":"File '") << s << "' on " << utils::inet_ntoa(request.address()) << endl;
	database->report_file(request, fileinfo);
}

// 
void thread_smb__file_flushaction (c_request request, c_path path)
{
	database->flush_files(request, path);
}



// options to scanner (per-process)
c_request  thread_smb__request;
int thread_smb ()
{
	int result = 0;
	c_request request = thread_smb__request;
	cerr << "************ ADDRESS " << utils::inet_ntoa(request.address()) << " ***********" << endl;
	// getting list of shares
	thread_smb__shares.clear();
	if (request.share().empty())
	{
		c_request sharerequest = request;
		sharerequest.depth(1);
		thread_wrap(options::command_smb, sharerequest, thread_smb__share_topaction,  thread_smb__share_subaction, thread_smb__share_flushaction);
	} else {
		c_path filepath; filepath.push_back(request.share());
		c_fileinfo fileinfo(filepath, true, 0, 0, 0);
		thread_smb__share_subaction(request, fileinfo);
	}
	// scanning each share for files with independent sequentive thread
	for (vector< pair<string,t_id> >::const_iterator i = thread_smb__shares.begin(); i != thread_smb__shares.end(); i++)
	{
		c_request sharerequest = request;
		sharerequest.share(i->first);
		sharerequest.resourceid(i->second);
		thread_wrap(options::command_smb, sharerequest, thread_smb__file_topaction, thread_smb__file_subaction, thread_smb__file_flushaction);
	}

	return result;
}
