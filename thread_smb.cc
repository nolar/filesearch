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


void thread_smb__action_resource (c_query query, c_path path)
{
	t_thread_smb__share item;
	item.first  = path.basename();
	item.second = database->report_share(query, item.first);
	thread_smb__shares.push_back(item);
}

void thread_smb__action_dir (c_query query, c_fileinfo fileinfo)
{
//!!!	string s; c_path p = fileinfo.path();
//!!!	for (c_path::const_iterator i = p.begin(); i != p.end(); i++) s += "/" + *i;
//!!!	cerr << (fileinfo.container()?"Dir  '":"File '") << s << "' on " << utils::inet_ntoa(query.address()) << endl;
	database->report_file(query, fileinfo);
}

void thread_smb__action_file (c_query query, c_fileinfo fileinfo)
{
//!!!	string s; c_path p = fileinfo.path();
//!!!	for (c_path::const_iterator i = p.begin(); i != p.end(); i++) s += "/" + *i;
//!!!	cerr << (fileinfo.container()?"Dir  '":"File '") << s << "' on " << utils::inet_ntoa(query.address()) << endl;
	database->report_file(query, fileinfo);
}

void thread_smb__action_dataflow (c_query query)
{
	database->status_renew(query);
}



// options to scanner (per-process)
c_query  thread_smb__query;
int thread_smb ()
{
	c_query query = thread_smb__query;
	std::string urlstrh = query.ascii();
	std::string urlstrs;
	int result = 0;
	// getting list of shares
	thread_smb__shares.clear();
	if (query.share().empty())
	{
		DEBUG("Scanning for shares on url '"+urlstrh+"'.");
		c_query sharequery = query;
		sharequery.depth(1);
		if (thread_wrap(default_scanner_smb, sharequery,
			thread_smb__action_resource,
			NULL,
			NULL,
			thread_smb__action_dataflow))
		{
			DEBUG("Flushing shares on url '"+urlstrh+"'.");
			database->flush_shares(sharequery);
		}
		DEBUG("Scanned for shares on url '"+urlstrh+"'. Got "+c_unsigned(thread_smb__shares.size()).ascii()+" shares.");
	} else {
		DEBUG("queryed share '"+query.share().ascii()+"' on url '"+urlstrh+"'.");
		c_path filepath(query.share().ascii(), default_path_delimiter);
		thread_smb__action_resource(query, filepath);
	}
	// scanning each share for files with independent sequentive thread
	for (t_thread_smb__shares::const_iterator i = thread_smb__shares.begin(); i != thread_smb__shares.end(); i++)
	{
		urlstrs = urlstrh + "/" + i->first;
		DEBUG("Scanning for files on url '"+urlstrs+"'.");
		c_query sharequery = query;
		sharequery.share(i->first);
		sharequery.resource(i->second);
		if (thread_wrap(default_scanner_smb, sharequery,
			NULL,
			thread_smb__action_dir,
			thread_smb__action_file,
			thread_smb__action_dataflow))
		{
			DEBUG("Flushing files on url '"+urlstrs+"'.");
			database->flush_files(sharequery);
		}
		DEBUG("Scanned for files on url '"+urlstrs+"'.");
	}
	return result;
}
