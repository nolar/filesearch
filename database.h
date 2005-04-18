#ifndef _FILESEARCH_DATABASE_
#define _FILESEARCH_DATABASE_

#include "request.h"
#include "utils.h"
#include "e_database.h"

using namespace std;

class c_database {
private:
protected:
public:
	c_database ();
	virtual ~c_database ();
	//
	virtual void thread_init () = 0;
	virtual void thread_free () = 0;
	// Functions to work with address blocks.
	virtual c_requests fetch_requests () = 0;
	// Functions to work with temporary per-process host/share status.
	virtual bool status_check (t_ipaddr address, t_proto proto, t_ipport port, string share, string username) = 0;
	virtual void status_renew (t_ipaddr address, t_proto proto, t_ipport port, string share, string username) = 0;
	virtual void status_clean () = 0;
	//
	virtual t_id report_item (c_request request, string share) = 0;
//	virtual t_id report_file (t_id item, c_path path, c_stat stat) = 0;
	//
};

#endif
