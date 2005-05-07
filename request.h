#ifndef _FILESEARCH_REQUEST_
#define _FILESEARCH_REQUEST_

#include <string>
#include <vector>
#include "config.h"
#include "typedefs.h"

using namespace std;

class c_request {
private:
	t_sqlid f_id;
	t_proto f_proto;
	t_flag f_isnetwork;
	t_ipaddr f_address;
	t_ipmask f_netmask;
	t_ipport f_port;
	string f_share;
	string f_username;

	string f_password;
	string f_workgroup;
	string f_selfname;
	t_timeout f_timeout;
	t_depth   f_depth;

	t_sqlid f_resourceid;
protected:
	static const t_ipaddr address_max;
	static const t_ipaddr address_min;
	static const t_ipaddr address_self;
	static const t_ipmask netmask_min;
	static const t_ipmask netmask_max;
	static const t_ipmask netmask_self;
public:
	c_request ();
//	c_request (t_sqlid   a_id, t_proto a_proto, bool   a_isnetwork, t_ipaddr a_address, t_ipmask a_netmask, t_ipport a_port, string a_share, string a_username, string a_password, string a_workgroup, string a_selfname, unsigned a_timeout, unsigned a_depth);
	c_request (string a_id, string  a_proto, string a_isnetwork, string   a_address, string   a_netmask, string   a_port, string a_share, string a_username, string a_password, string a_workgroup, string a_selfname, string   a_timeout, string   a_depth);

	c_request & operator = (const c_request & right);

	t_sqlid id ();
	t_proto proto ();
	bool isnetwork ();
	t_ipaddr address ();
	void address (t_ipaddr address);
	t_ipmask netmask ();
	t_ipaddr address_from ();
	t_ipaddr address_till ();
	t_ipport port ();
	string share ();
	void share (string value);
	string username ();
	string password ();
	string workgroup ();
	string selfname ();
	t_timeout timeout ();
	t_depth   depth ();
	void depth (t_depth value);

	t_sqlid resourceid ();
	void resourceid (t_sqlid id);
};

typedef vector<c_request> c_requests;

#endif
