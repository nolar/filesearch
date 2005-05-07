#include "request.h"
#include "convert.h"
#include "options.h"


const t_ipaddr c_request::address_max  = (t_ipaddr) 0xFFFFFFFF;
const t_ipaddr c_request::address_min  = (t_ipaddr) 0;
const t_ipaddr c_request::address_self = (t_ipaddr) 0x7F000001;
const t_ipmask c_request::netmask_min  = (t_ipmask) 0;
const t_ipmask c_request::netmask_max  = (t_ipmask) 32;
const t_ipmask c_request::netmask_self = (t_ipaddr) 32;



c_request::c_request ()
{
}

/*c_request::c_request (t_proto a_proto, bool   a_isnetwork, t_ipaddr a_address, t_ipmask a_netmask, t_ipport a_port, string a_share, string a_username, string a_password, string a_workgroup, string a_selfname, unsigned a_timeout, unsigned a_depth)
{
	f_proto     = a_proto;
	f_isnetwork = a_isnetwork;
	f_address   = a_address;
	f_netmask   = a_netmask;
	f_port      = a_port;
	f_share     = a_share;
	f_username  = a_username;
	f_password  = a_password;
	f_workgroup = a_workgroup;
	f_selfname  = a_selfname;
	f_timeout   = a_timeout;
	f_depth     = a_depth;
}*/

c_request::c_request (string a_id, string  a_proto, string a_isnetwork, string   a_address, string   a_netmask, string   a_port, string a_share, string a_username, string a_password, string a_workgroup, string a_selfname, string   a_timeout, string  a_depth)
{
	f_id        = convert::str2sqlid(a_id);
	f_proto     = convert::str2proto(a_proto, proto_unknown);
	f_isnetwork = convert::str2flag(a_isnetwork, false);
	f_address   = convert::str2ipaddr(a_address, address_self);
	f_netmask   = convert::str2ipmask(a_netmask, netmask_self);
	f_port      = convert::str2ipport(a_port, 0);
	f_share     = a_share;
	f_username  = a_username;
	f_password  = a_password;
	f_workgroup = a_workgroup;
	f_selfname  = a_selfname;
	f_timeout   = convert::str2timeout(a_timeout, options::default_timeout);
	f_depth     = convert::str2depth  (a_depth  , options::default_depth  );
}

c_request & c_request::operator= (const c_request & right)
{
	f_id        = right.f_id;
	f_proto     = right.f_proto;
	f_isnetwork = right.f_isnetwork;
	f_address   = right.f_address;
	f_netmask   = right.f_netmask;
	f_port      = right.f_port;
	f_share     = right.f_share;
	f_username  = right.f_username;
	f_password  = right.f_password;
	f_workgroup = right.f_workgroup;
	f_selfname  = right.f_selfname;
	f_timeout   = right.f_timeout;
	f_depth     = right.f_depth;
	return *this;
}


t_sqlid c_request::id ()
{
	return f_id;
}

t_proto c_request::proto ()
{
	return f_proto;
}

bool c_request::isnetwork ()
{
	return f_isnetwork;
}

t_ipaddr c_request::address ()
{
	return f_address;
}

void c_request::address (t_ipaddr address)
{
	f_address   = address;
	f_netmask   = netmask_max;
	f_isnetwork = false;
}

t_ipmask c_request::netmask ()
{
	return f_netmask;
}

t_ipaddr c_request::address_from ()
{
	t_ipaddr result = f_netmask ?
		(f_address >> (netmask_max-f_netmask)) << (netmask_max-f_netmask)
		: address_min;
	return f_isnetwork ? result + 1 : result;
}

t_ipaddr c_request::address_till ()
{
	t_ipaddr result = f_netmask ? (address_from() |~
		((address_max >> (netmask_max-f_netmask)) << (netmask_max-f_netmask)))
		: address_max;
	return f_isnetwork ? result - 1 : result;
}

t_ipport c_request::port ()
{
	return f_port;
}

string c_request::share ()
{
	return f_share;
}

void c_request::share (string value)
{
	f_share = value;
}

string c_request::username ()
{
	return f_username;
}

string c_request::password ()
{
	return f_password;
}

string c_request::workgroup ()
{
	return f_workgroup;
}

string c_request::selfname ()
{
	return f_selfname;
}

t_timeout c_request::timeout ()
{
	return f_timeout;
}

t_depth c_request::depth ()
{
	return f_depth;
}

void c_request::depth (t_depth value)
{
	f_depth = value;
}




t_sqlid c_request::resourceid ()
{
	return f_resourceid;
}

void c_request::resourceid (t_sqlid id)
{
	f_resourceid = id;
}
