#ifndef _FILESEARCH_DATABASE_
#define _FILESEARCH_DATABASE_

#include "config.h"
#include "typedefs.h"
#include "e_database.h"
#include "request.h"
#include "fileinfo.h"

using namespace std;

class c_database {
private:
protected:
	// fields for status maintenance
	t_pident  f_process;
	t_time f_startup;
public:
	c_database (t_pident a_process, t_time a_startup);
	virtual ~c_database ();
	//
	virtual c_database * duplicate () = 0;

	// Functions to work with address blocks.
	virtual c_requests fetch_requests () = 0;

	// Functions to work with temporary per-process host/share status.
	virtual bool status_check (c_request request) = 0;
	virtual void status_renew (c_request request) = 0;
	virtual void status_clean () = 0;

	//
	virtual t_sqlid report_share (c_request request, string share) = 0;
	virtual t_sqlid report_file  (c_request request, c_fileinfo fileinfo) = 0;
	virtual void    flush_shares (c_request request) = 0;
	virtual void    flush_files  (c_request request) = 0;
};

#endif
