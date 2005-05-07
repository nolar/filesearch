#ifndef _FILESEARCH_DATABASE_MYSQL_
#define _FILESEARCH_DATABASE_MYSQL_

#include "database.h"
#include "e_database.h"
#include <mysql.h>

class c_database_mysql : public c_database {
private:
	MYSQL handle;
	//
	vector<t_sqlid> _resource_cache;
	bool _resource_find  (t_sqlid &id, bool &changed, c_request request, string share);
	bool _resource_add   (t_sqlid &id, c_request request, string share);
	void _resource_loose (c_request request);
	void _resource_loosf (c_request request);
	void _resource_touch (vector<t_sqlid> ids);
	void _resource_flush (bool forced);

	vector<t_sqlid> _file_cache;
	bool _file_find   (t_sqlid &id, bool &changed, c_request request, c_fileinfo fileinfo);
	bool _file_add    (t_sqlid &id, c_request request, c_fileinfo fileinfo);
	void _file_loose  (c_request request);
	void _file_touch  (vector<t_sqlid> ids);
	void _file_flush  (bool forced);
	void _file_change (t_sqlid id, c_fileinfo fileinfo);

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
	static t_pident _initialized_pid;
	// supplimentary routines
	string escape (string s);
	string quote  (string s);
///	void thread_lock ();
///	void thread_unlock ();
public:
	c_database_mysql (t_pident a_process, t_time a_startup, string host, string user, string pass, string db, unsigned int port, string socket, unsigned long flags);
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
	virtual t_sqlid report_share (c_request request, string share);
	virtual t_sqlid report_file  (c_request request, c_fileinfo fileinfo);
	virtual void flush_shares (c_request request);
	virtual void flush_files  (c_request request);
};

#endif
