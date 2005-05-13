#include <string.h>
#include "c_unsigned.h"

c_unsigned::c_unsigned ()
	: f_value()
{
}

c_unsigned::c_unsigned (unsigned value)
	: f_value(value)
{
}



t_object_size c_unsigned::stream_size () const
{
	return sizeof(f_value);
}

void c_unsigned::stream_getdata (void * buffer, t_object_size size) const
{
	memcpy(buffer, &f_value, sizeof(f_value));
}

void c_unsigned::stream_setdata (const void * buffer, t_object_size size)
{
	memcpy(&f_value, buffer, sizeof(f_value));
}




unsigned c_unsigned::get ()
{
	return f_value;
}

void c_unsigned::set (unsigned value)
{
	f_value = value;
}


bool c_unsigned::operator== (const c_unsigned & right) const
{
	return f_value == right.f_value;
}

bool c_unsigned::operator<  (const c_unsigned & right) const
{
	return f_value < right.f_value;
}
