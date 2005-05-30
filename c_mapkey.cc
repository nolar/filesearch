#include <string.h>
#include "c_mapkey.h"

c_mapkey::c_mapkey ()
	: c_object()
	, f_value()
{
}

c_mapkey::c_mapkey (int value)
	: c_object()
	, f_value(value)
{
}




bool c_mapkey::stream_vary () const
{
	return false;
}

t_object_size c_mapkey::stream_size () const
{
	return sizeof(f_value);
}

void c_mapkey::stream_getdata (void * buffer, t_object_size size) const
{
	memcpy(buffer, &f_value, sizeof(f_value));
}

void c_mapkey::stream_setdata (const void * buffer, t_object_size size)
{
	memcpy(&f_value, buffer, sizeof(f_value));
}




int c_mapkey::get () const
{
	return f_value;
}

void c_mapkey::set (int value)
{
	f_value = value;
}


bool c_mapkey::operator== (const c_mapkey & right) const
{
	return f_value == right.f_value;
}

bool c_mapkey::operator<  (const c_mapkey & right) const
{
	return f_value < right.f_value;
}
