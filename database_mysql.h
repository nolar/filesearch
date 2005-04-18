#ifndef _FILESEARCH_DATABASE_MYSQL_
#define _FILESEARCH_DATABASE_MYSQL_

#include "database.h"
#include "e_database.h"
#include <mysql.h>

class c_database_mysql : public c_database {
private:
	MYSQL handle;
	bool _select_host (t_id &id, t_ipaddr address);
	bool _insert_host (t_id &id, t_ipaddr address);
	void _update_host (t_id id);
	bool _select_item (t_id &id, t_id host, t_proto proto, t_ipport port, string username, string password, string hostname, string netbiosshare, string netbiosname, string netbiosgroup);
	bool _insert_item (t_id &id, t_id host, t_proto proto, t_ipport port, string username, string password, string hostname, string netbiosshare, string netbiosname, string netbiosgroup);
	void _update_item (t_id id);

	bool _select_file (t_id &id, t_id item, string path, string name);
	bool _insert_file (t_id &id, t_id item, string path, string name);
	void _update_file (t_id id, size_t size, time_t time);
protected:
	time_t f_startup;
	pid_t f_process;
	string escape (string s);
	void thread_lock ();
	void thread_unlock ();
public:
	c_database_mysql (string host, string user, string pass, string db, unsigned int port, string socket, unsigned long flags);
	virtual ~c_database_mysql ();
	//
	virtual void thread_init ();
	virtual void thread_free ();
	// Functions to work with address requests.
	virtual c_requests fetch_requests ();
	// Functions to work with temporary per-process host/share status.
	virtual bool status_check (t_ipaddr address, t_proto proto, t_ipport port, string share, string username);
	virtual void status_renew (t_ipaddr address, t_proto proto, t_ipport port, string share, string username);
	virtual void status_clean ();
	//
	virtual t_id report_item (c_request request, string share);
//	virtual t_id report_file (t_id item, c_path path, c_stat stat);
};

#endif
