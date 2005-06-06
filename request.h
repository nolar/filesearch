#ifndef _FILESEARCH_REQUEST_
#define _FILESEARCH_REQUEST_

#include <string>
#include <vector>
#include "config.h"
#include "globals.h"

#include "c_string.h"
#include "c_flag.h"
#include "c_unsigned.h"
#include "c_protocol.h"
#include "c_ipaddr.h"

class c_request {
private:
	c_unsigned f_id;
	c_protocol f_protocol;
	c_flag     f_isnetwork;
	c_ipaddr   f_ipaddr;
	c_unsigned f_ipport;
	c_string   f_share;
	c_string   f_username;
	c_string   f_password;
	c_string   f_workgroup;
	c_string   f_selfname;
	c_unsigned f_timeout;
	c_unsigned f_depth;
	c_unsigned f_resource;
protected:
public:
	c_request ();
	c_request (const c_request & right);
	c_request (c_unsigned a_id, c_protocol a_protocol, c_flag a_isnetwork, c_ipaddr a_ipaddr, c_unsigned a_ipport, c_string a_share, c_string a_username, c_string a_password, c_string a_workgroup, c_string a_selfname, c_unsigned a_timeout, c_unsigned a_depth);

	std::string ascii() const;

	c_unsigned id        ();
	c_protocol protocol  ();
	c_flag     isnetwork (); void isnetwork (c_flag     value);
	c_ipaddr   ipaddr    (); void ipaddr    (c_ipaddr   value);
	c_unsigned ipport    ();
	c_string   share     (); void share     (c_string   value);
	c_string   username  ();
	c_string   password  ();
	c_string   workgroup ();
	c_string   selfname  ();
	c_unsigned timeout   ();
	c_unsigned depth     (); void depth     (c_unsigned value);
	c_unsigned resource  (); void resource  (c_unsigned value);
};

typedef std::vector<c_request> c_requests;

#endif
