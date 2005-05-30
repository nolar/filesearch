#include <string.h>
#include "c_protocol.h"
#include "globals.h"

c_protocol::c_protocol ()
	: c_object()
	, f_value()
{
}

c_protocol::c_protocol (c_protocol::t_value value)
	: c_object()
	, f_value(value)
{
}



bool c_protocol::stream_vary () const
{
	return false;
}

t_object_size c_protocol::stream_size () const
{
	return sizeof(f_value);
}

void c_protocol::stream_getdata (void * buffer, t_object_size size) const
{
	memcpy(buffer, &f_value, sizeof(f_value));
}

void c_protocol::stream_setdata (const void * buffer, t_object_size size)
{
	memcpy(&f_value, buffer, sizeof(f_value));
}




c_protocol::t_value c_protocol::get ()
{
	return f_value;
}

void c_protocol::set (c_protocol::t_value value)
{
	f_value = value;
}

const char * c_protocol::ascii () const
{
	switch (f_value)
	{
		case  1: return "smb" ;
		case  2: return "ftp" ;
		case  3: return "http";
		default: return "unknown";
	}
}



bool c_protocol::operator== (const c_protocol & right) const
{
	return f_value == right.f_value;
}
