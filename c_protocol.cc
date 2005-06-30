#include "c_protocol.h"
#include "globals.h"
#include <string.h>





const c_protocol::t_value c_protocol::value_smb  = 1;
const c_protocol::t_value c_protocol::value_ftp  = 2;
const c_protocol::t_value c_protocol::value_http = 3;





c_protocol::c_protocol ()
	: c_object()
	, f_value()
{
}

c_protocol::c_protocol (const c_protocol & right)
	: c_object()
	, f_value(right.f_value)
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





c_protocol & c_protocol::operator=(const c_protocol & right)
{
	f_value = right.f_value;
	return *this;
}

bool c_protocol::operator== (const c_protocol & right) const
{
	return f_value == right.f_value;
}

bool c_protocol::operator!= (const c_protocol & right) const
{
	return f_value != right.f_value;
}





c_protocol::t_value c_protocol::get (void) const
{
	return f_value;
}

void c_protocol::set (c_protocol::t_value value)
{
	f_value = value;
}

std::string c_protocol::ascii () const
{
	switch (f_value)
	{
		case value_smb : return std::string("smb" );
		case value_ftp : return std::string("ftp" );
		case value_http: return std::string("http");
		default: return std::string("unknown") + "(" + string_format("%d", f_value) + ")";
	}
}





bool c_protocol::is_smb () const
{
	return f_value == value_smb;
}

bool c_protocol::is_ftp () const
{
	return f_value == value_ftp;
}

bool c_protocol::is_http () const
{
	return f_value == value_http;
}
