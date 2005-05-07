#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "thread_smb.h"
#include "io.h"
#include "thread.h"
#include "options.h"
#include "database.h"
#include "e_database.h"
#include "convert.h"

extern c_database * database;

//
vector< pair<string,t_sqlid> > thread_smb__shares;


void thread_smb__action_resource (c_request request, t_path path)
{
	pair<string,t_sqlid> item;
	item.first  = path.empty()?string():path[path.size()-1];
	item.second = database->report_share(request, item.first);
	thread_smb__shares.push_back(item);
}

void thread_smb__action_dir (c_request request, c_fileinfo fileinfo)
{
//!!!	string s; c_path p = fileinfo.path();
//!!!	for (c_path::const_iterator i = p.begin(); i != p.end(); i++) s += "/" + *i;
//!!!	cerr << (fileinfo.container()?"Dir  '":"File '") << s << "' on " << utils::inet_ntoa(request.address()) << endl;
	database->report_file(request, fileinfo);
}

void thread_smb__action_file (c_request request, c_fileinfo fileinfo)
{
//!!!	string s; c_path p = fileinfo.path();
//!!!	for (c_path::const_iterator i = p.begin(); i != p.end(); i++) s += "/" + *i;
//!!!	cerr << (fileinfo.container()?"Dir  '":"File '") << s << "' on " << utils::inet_ntoa(request.address()) << endl;
	database->report_file(request, fileinfo);
}

void thread_smb__action_start (c_request request)
{
	database->status_renew(request);
}



// options to scanner (per-process)
c_request  thread_smb__request;
int thread_smb ()
{
	int result = 0;
	c_request request = thread_smb__request;
	// getting list of shares
	thread_smb__shares.clear();
	if (request.share().empty())
	{
		DEBUG("(smb) Scanning for shares on ip='"+convert::ipaddr2print(request.address())+"',username='"+request.username()+"'.");
		c_request sharerequest = request;
		sharerequest.depth(1);
		if (thread_wrap(options::command_smb, sharerequest,
			thread_smb__action_resource,
			NULL,
			NULL,
			NULL,
			NULL,
			thread_smb__action_start))
		{
			DEBUG("(smb) Flushing shares on ip='"+convert::ipaddr2print(request.address())+"',username='"+request.username()+"'.");
			database->flush_shares(sharerequest);
		}
		DEBUG("(smb) Scanned for shares on ip='"+convert::ipaddr2print(request.address())+"',username='"+request.username()+"'. Got "+convert::ui2str(thread_smb__shares.size())+" shares.");
	} else {
		DEBUG("(smb) Requested share '"+request.share()+"' on ip='"+convert::ipaddr2print(request.address())+"',username='"+request.username()+"'.");
		t_path filepath; filepath.push_back(request.share());
		thread_smb__action_resource(request, filepath);
	}
	// scanning each share for files with independent sequentive thread
	for (vector< pair<string,t_sqlid> >::const_iterator i = thread_smb__shares.begin(); i != thread_smb__shares.end(); i++)
	{
		DEBUG("(smb) Scanning for files on ip='"+convert::ipaddr2print(request.address())+"',share='"+i->first+"',username='"+request.username()+"'.");
		c_request sharerequest = request;
		sharerequest.share(i->first);
		sharerequest.resourceid(i->second);
		if (thread_wrap(options::command_smb, sharerequest,
			NULL,
			thread_smb__action_dir,
			thread_smb__action_file,
			NULL,
			NULL,
			thread_smb__action_start))
		{
			DEBUG("(smb) Flushing files on ip='"+convert::ipaddr2print(request.address())+"',share='"+i->first+"',username='"+request.username()+"'.");
			database->flush_files(sharerequest);
		}
		DEBUG("(smb) Scanned for files on ip='"+convert::ipaddr2print(request.address())+"',share='"+i->first+"',username='"+request.username()+"'.");
	}
	return result;
}
