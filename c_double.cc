#include <string.h>
#include "c_double.h"

c_double::c_double ()
	: f_value()
{
}

c_double::c_double (double value)
	: f_value(value)
{
}



t_object_size c_double::stream_size () const
{
	return sizeof(f_value);
}

void c_double::stream_getdata (void * buffer, t_object_size size) const
{
	memcpy(buffer, &f_value, sizeof(f_value));
}

void c_double::stream_setdata (const void * buffer, t_object_size size)
{
	memcpy(&f_value, buffer, sizeof(f_value));
}




double c_double::get ()
{
	return f_value;
}

void c_double::set (double value)
{
	f_value = value;
}


bool c_double::operator== (const c_double & right) const
{
	return f_value == right.f_value;
}

bool c_double::operator<  (const c_double & right) const
{
	return f_value < right.f_value;
}
