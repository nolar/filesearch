#ifndef _FILESEARCH_DATABASE_
#define _FILESEARCH_DATABASE_

#include "request.h"
#include "fileinfo.h"
#include "utils.h"
#include "e_database.h"

using namespace std;

class c_database {
private:
protected:
	// fields for status maintenance
	pid_t  f_process;
	time_t f_startup;
public:
	c_database (pid_t a_process, time_t a_startup);
	virtual ~c_database ();
	//
	virtual c_database * duplicate () = 0;
	//
///	virtual void thread_init () = 0;
///	virtual void thread_free () = 0;
	// Functions to work with address blocks.
	virtual c_requests fetch_requests () = 0;
	// Functions to work with temporary per-process host/share status.
	virtual bool status_check (c_request request) = 0;
	virtual void status_renew (c_request request) = 0;
	virtual void status_clean () = 0;
	//
	virtual t_id report_share (c_request request, string share) = 0;
	virtual t_id report_file  (c_request request, c_fileinfo fileinfo) = 0;
	virtual void flush_shares (c_request request) = 0;
	virtual void flush_files  (c_request request) = 0;
};

#endif
