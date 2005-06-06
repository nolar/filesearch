#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "thread_smb.h"
#include "c_stream.h"
#include "thread.h"
#include "globals.h"
#include "database.h"

extern c_database * database;

//
typedef std::pair<std::string,c_unsigned> t_thread_smb__share;
typedef std::vector<t_thread_smb__share> t_thread_smb__shares;
t_thread_smb__shares thread_smb__shares;


void thread_smb__action_resource (c_request request, c_path path)
{
	t_thread_smb__share item;
	item.first  = path.basename();
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

void thread_smb__action_dataflow (c_request request)
{
	database->status_renew(request);
}



// options to scanner (per-process)
c_request  thread_smb__request;
int thread_smb ()
{
	c_request request = thread_smb__request;
	std::string urlstrh = request.ascii();
	std::string urlstrs;
	int result = 0;
	// getting list of shares
	thread_smb__shares.clear();
	if (request.share().empty())
	{
		DEBUG("Scanning for shares on url '"+urlstrh+"'.");
		c_request sharerequest = request;
		sharerequest.depth(1);
		if (thread_wrap(default_scanner_smb, sharerequest,
			thread_smb__action_resource,
			NULL,
			NULL,
			thread_smb__action_dataflow))
		{
			DEBUG("Flushing shares on url '"+urlstrh+"'.");
			database->flush_shares(sharerequest);
		}
		DEBUG("Scanned for shares on url '"+urlstrh+"'. Got "+c_unsigned(thread_smb__shares.size()).ascii()+" shares.");
	} else {
		DEBUG("Requested share '"+request.share().ascii()+"' on url '"+urlstrh+"'.");
		c_path filepath(request.share().ascii(), default_path_delimiter);
		thread_smb__action_resource(request, filepath);
	}
	// scanning each share for files with independent sequentive thread
	for (t_thread_smb__shares::const_iterator i = thread_smb__shares.begin(); i != thread_smb__shares.end(); i++)
	{
		urlstrs = urlstrh + "/" + i->first;
		DEBUG("Scanning for files on url '"+urlstrs+"'.");
		c_request sharerequest = request;
		sharerequest.share(i->first);
		sharerequest.resource(i->second);
		if (thread_wrap(default_scanner_smb, sharerequest,
			NULL,
			thread_smb__action_dir,
			thread_smb__action_file,
			thread_smb__action_dataflow))
		{
			DEBUG("Flushing files on url '"+urlstrs+"'.");
			database->flush_files(sharerequest);
		}
		DEBUG("Scanned for files on url '"+urlstrs+"'.");
	}
	return result;
}
