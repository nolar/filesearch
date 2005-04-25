#ifndef _FILESEARCH_DATABASE_MYSQL_
#define _FILESEARCH_DATABASE_MYSQL_

#include "database.h"
#include "e_database.h"
#include <mysql.h>

class c_database_mysql : public c_database {
private:
	MYSQL handle;
	//
	bool _select_resource (t_id &id, c_request request, string share);
	bool _insert_resource (t_id &id, c_request request, string share);
	void _update_resource (t_id  id);
	void _loose_resources (c_request request);

	bool _select_file (t_id &id, c_request request, c_fileinfo fileinfo);
	bool _insert_file (t_id &id, c_request request, c_fileinfo fileinfo);
	void _update_file (t_id id, c_fileinfo fileinfo);
	void _loose_files (c_request request, string path);
protected:
	//
	string f_host;
	string f_user;
	string f_pass;
	string f_db;
	unsigned int f_port;
	string f_socket;
	unsigned long f_flags;
	// field for per-thread initialization
	static pid_t _initialized_pid;
	// supplimentary routines
	string escape (string s);
///	void thread_lock ();
///	void thread_unlock ();
public:
	c_database_mysql (pid_t a_process, time_t a_startup, string host, string user, string pass, string db, unsigned int port, string socket, unsigned long flags);
	virtual ~c_database_mysql ();
	//
	virtual c_database * duplicate ();
	//
///	virtual void thread_init ();
///	virtual void thread_free ();
	// Functions to work with address requests.
	virtual c_requests fetch_requests ();
	// Functions to work with temporary per-process host/share status.
	virtual bool status_check (c_request request);
	virtual void status_renew (c_request request);
	virtual void status_clean ();
	//
	virtual t_id report_share (c_request request, string share);
	virtual t_id report_file  (c_request request, c_fileinfo fileinfo);
	virtual void flush_shares (c_request request);
	virtual void flush_files  (c_request request, c_path path);
};

#endif
