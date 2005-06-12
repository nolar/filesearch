#include "query.h"

c_query::c_query ()
{
}

c_query::c_query (const c_query & right)
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

c_query::c_query (c_unsigned a_id, c_protocol a_protocol, c_flag a_isnetwork, c_ipaddr a_ipaddr, c_unsigned a_ipport, c_string a_share, c_string a_username, c_string a_password, c_string a_workgroup, c_string a_selfname, c_unsigned a_timeout, c_unsigned a_depth)
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




std::string c_query::ascii() const
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




c_unsigned c_query::id ()
{
	return f_id;
}

c_protocol c_query::protocol ()
{
	return f_protocol;
}

c_flag c_query::isnetwork ()
{
	return f_isnetwork;
}

void c_query::isnetwork (c_flag value)
{
	f_isnetwork = value;
}

c_ipaddr c_query::ipaddr ()
{
	return f_ipaddr;
}

void c_query::ipaddr (c_ipaddr value)
{
	f_ipaddr = value;
}

c_unsigned c_query::ipport ()
{
	return f_ipport;
}

c_string c_query::share ()
{
	return f_share;
}

void c_query::share (c_string value)
{
	f_share = value;
}

c_string c_query::username ()
{
	return f_username;
}

c_string c_query::password ()
{
	return f_password;
}

c_string c_query::workgroup ()
{
	return f_workgroup;
}

c_string c_query::selfname ()
{
	return f_selfname;
}

c_unsigned c_query::timeout ()
{
	return f_timeout;
}

c_unsigned c_query::depth ()
{
	return f_depth;
}

void c_query::depth (c_unsigned value)
{
	f_depth = value;
}

c_unsigned c_query::resource ()
{
	return f_resource;
}

void c_query::resource (c_unsigned value)
{
	f_resource = value;
}
