#include <string.h>
#include "c_string.h"

c_string::c_string ()
	: f_value()
{
}

c_string::c_string (std::string value)
	: f_value(value)
{
}

c_string::c_string (char * value)
	: f_value(value)
{
}



t_object_size c_string::stream_size () const
{
	return f_value.length();
}

void c_string::stream_getdata (void * buffer, t_object_size size) const
{
	memcpy(buffer, f_value.c_str(), f_value.length());
}

void c_string::stream_setdata (const void * buffer, t_object_size size)
{
	f_value = std::string(static_cast<const std::string::value_type*>(buffer), size);
}




std::string c_string::get () const
{
	return f_value;
}

void c_string::set (std::string value)
{
	f_value = value;
}

const char * c_string::c_str () const
{
	return f_value.c_str();
}

bool c_string::empty () const
{
	return f_value.empty();
}




bool c_string::operator== (const c_string & right) const
{
	return f_value == right.f_value;
}

bool c_string::operator<  (const c_string & right) const
{
	return f_value < right.f_value;
}
