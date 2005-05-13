#include <string.h>
#include "c_signed.h"

c_signed::c_signed ()
	: f_value()
{
}

c_signed::c_signed (signed value)
	: f_value(value)
{
}



t_object_size c_signed::stream_size () const
{
	return sizeof(f_value);
}

void c_signed::stream_getdata (void * buffer, t_object_size size) const
{
	memcpy(buffer, &f_value, sizeof(f_value));
}

void c_signed::stream_setdata (const void * buffer, t_object_size size)
{
	memcpy(&f_value, buffer, sizeof(f_value));
}




signed c_signed::get ()
{
	return f_value;
}

void c_signed::set (signed value)
{
	f_value = value;
}


bool c_signed::operator== (const c_signed & right) const
{
	return f_value == right.f_value;
}

bool c_signed::operator<  (const c_signed & right) const
{
	return f_value < right.f_value;
}
