#ifndef _FILESEARCH_REQUEST_
#define _FILESEARCH_REQUEST_

#include <string>
#include <vector>
#include "utils.h"

using namespace std;

class c_request {
private:
	t_proto f_proto;
	bool f_isnetwork;
	t_ipaddr f_address;
	t_ipmask f_netmask;
	t_ipport f_port;
	string f_share;
	string f_username;

	string f_password;
	string f_workgroup;
	string f_selfname;
	unsigned f_timeout;
protected:
	static const unsigned long address_max = (t_ipaddr) -1;
	static const unsigned long address_min = (t_ipaddr) 0;
	static const unsigned long netmask_min = (t_ipmask) 0;
	static const unsigned long netmask_max = (t_ipmask) 32;
public:
	c_request ();
	c_request (t_proto a_proto, bool   a_isnetwork, t_ipaddr a_address, t_ipmask a_netmask, t_ipport a_port, string a_share, string a_username, string a_password, string a_workgroup, string a_selfname, unsigned a_timeout);
	c_request (string  a_proto, string a_isnetwork, string   a_address, string   a_netmask, string   a_port, string a_share, string a_username, string a_password, string a_workgroup, string a_selfname, string   a_timeout);

	c_request & operator = (const c_request & right);
	c_request & operator = (const t_ipaddr & address);

	t_proto proto ();
	bool isnetwork ();
	t_ipaddr address ();
	t_ipmask netmask ();
	t_ipaddr address_from ();
	t_ipaddr address_till ();
	t_ipport port ();
	string share ();
	string username ();
	string password ();
	string workgroup ();
	string selfname ();
	unsigned timeout ();
};

typedef vector<c_request> c_requests;

#endif
