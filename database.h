#ifndef _FILESEARCH_DATABASE_
#define _FILESEARCH_DATABASE_

#include "config.h"
#include "globals.h"
#include "e_database.h"
#include "query.h"
#include "fileinfo.h"

class c_database {
private:
protected:
	// fields for status maintenance
	c_unsigned f_process;
	c_stamp    f_startup;
public:
	c_database ();
	virtual ~c_database ();
	virtual c_database * duplicate () = 0;

	// Functions to fetch basic data
	virtual c_stamp    fetch_startup  () = 0;
	virtual c_queries fetch_queries () = 0;

	// Functions to work with temporary per-process host/share status.
	        void status_init  ();
	        void status_init  (c_unsigned a_process, c_stamp a_startup);
	virtual bool status_check (c_query query) = 0;
	virtual void status_renew (c_query query) = 0;
	virtual void status_clean () = 0;

	//
	virtual c_unsigned report_share (c_query query, c_string share) = 0;
	virtual c_unsigned report_file  (c_query query, c_fileinfo fileinfo) = 0;
	virtual void       flush_shares (c_query query) = 0;
	virtual void       flush_files  (c_query query) = 0;
};

#endif
