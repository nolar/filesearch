#include <string.h>
#include "c_action.h"

c_action::c_action ()
	: c_object()
	, f_value()
{
}

c_action::c_action (unsigned char value)
	: c_object()
	, f_value(value)
{
}

c_action::c_action (const c_action & right)
	: c_object()
	, f_value(right.f_value)
{
}



bool c_action::stream_vary () const
{
	return false;
}

t_object_size c_action::stream_size () const
{
	return sizeof(f_value);
}

void c_action::stream_getdata (void * buffer, t_object_size size) const
{
	memcpy(buffer, &f_value, sizeof(f_value));
}

void c_action::stream_setdata (const void * buffer, t_object_size size)
{
	memcpy(&f_value, buffer, sizeof(f_value));
}




unsigned char c_action::get () const
{
	return f_value;
}

void c_action::set (unsigned char value)
{
	f_value = value;
}


bool c_action::operator== (const c_action & right) const
{
	return f_value == right.f_value;
}
