#include <string.h>
#include "c_flag.h"

c_flag::c_flag ()
	: f_value()
{
}

c_flag::c_flag (bool value)
	: f_value(value)
{
}



t_object_size c_flag::stream_size () const
{
	return sizeof(f_value);
}

void c_flag::stream_getdata (void * buffer, t_object_size size) const
{
	memcpy(buffer, &f_value, sizeof(f_value));
}

void c_flag::stream_setdata (const void * buffer, t_object_size size)
{
	memcpy(&f_value, buffer, sizeof(f_value));
}




bool c_flag::get ()
{
	return f_value;
}

void c_flag::set (bool value)
{
	f_value = value;
}


bool c_flag::operator== (const c_flag & right) const
{
	return f_value == right.f_value;
}

bool c_flag::operator<  (const c_flag & right) const
{
	return f_value < right.f_value;
}
