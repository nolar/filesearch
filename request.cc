#include "request.h"

c_request::c_request ()
{
}

c_request::c_request (const c_request & right)
{
	f_id        = right.f_id;
	f_protocol  = right.f_protocol;
	f_isnetwork = right.f_isnetwork;
	f_ipaddr    = right.f_ipaddr;
	f_ipport    = right.f_ipport;
	f_share     = right.f_share;
	f_username  = right.f_username;
	f_password  = right.f_password;
	f_workgroup = right.f_workgroup;
	f_selfname  = right.f_selfname;
	f_timeout   = right.f_timeout;
	f_depth     = right.f_depth;
	f_resource  = right.f_resource;
}

c_request::c_request (c_unsigned a_id, c_protocol a_protocol, c_flag a_isnetwork, c_ipaddr a_ipaddr, c_unsigned a_ipport, c_string a_share, c_string a_username, c_string a_password, c_string a_workgroup, c_string a_selfname, c_unsigned a_timeout, c_unsigned a_depth)
{
	f_id        = a_id;
	f_protocol  = a_protocol;
	f_isnetwork = a_isnetwork;
	f_ipaddr    = a_ipaddr;
	f_ipport    = a_ipport;
	f_share     = a_share;
	f_username  = a_username;
	f_password  = a_password;
	f_workgroup = a_workgroup;
	f_selfname  = a_selfname;
	f_timeout   = a_timeout;
	f_depth     = a_depth;
}




std::string c_request::ascii() const
{
	std::string result = std::string() + f_protocol.ascii() + "://";
	if (!f_username.empty())
	{
		result = result + f_username.ascii() + "@";
	}
	result = result + f_ipaddr.ascii();
	if (f_ipport != 0)
	{
		result = result + ":" + f_ipport.ascii();
	}
	result = result + "/" + f_share.ascii();
	return result;
}




c_unsigned c_request::id ()
{
	return f_id;
}

c_protocol c_request::protocol ()
{
	return f_protocol;
}

c_flag c_request::isnetwork ()
{
	return f_isnetwork;
}

void c_request::isnetwork (c_flag value)
{
	f_isnetwork = value;
}

c_ipaddr c_request::ipaddr ()
{
	return f_ipaddr;
}

void c_request::ipaddr (c_ipaddr value)
{
	f_ipaddr = value;
}

c_unsigned c_request::ipport ()
{
	return f_ipport;
}

c_string c_request::share ()
{
	return f_share;
}

void c_request::share (c_string value)
{
	f_share = value;
}

c_string c_request::username ()
{
	return f_username;
}

c_string c_request::password ()
{
	return f_password;
}

c_string c_request::workgroup ()
{
	return f_workgroup;
}

c_string c_request::selfname ()
{
	return f_selfname;
}

c_unsigned c_request::timeout ()
{
	return f_timeout;
}

c_unsigned c_request::depth ()
{
	return f_depth;
}

void c_request::depth (c_unsigned value)
{
	f_depth = value;
}

c_unsigned c_request::resource ()
{
	return f_resource;
}

void c_request::resource (c_unsigned value)
{
	f_resource = value;
}
