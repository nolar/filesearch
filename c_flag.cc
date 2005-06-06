#include "c_flag.h"
#include "globals.h"
#include <string.h>





c_flag::c_flag ()
	: c_object()
	, f_value()
{
}

c_flag::c_flag (const c_flag & right)
	: c_object()
	, f_value(right.f_value)
{
}

c_flag::c_flag (bool value)
	: c_object()
	, f_value(value)
{
}





bool c_flag::stream_vary () const
{
	return false;
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





c_flag & c_flag::operator= (const c_flag & right)
{
	f_value = right.f_value;
	return *this;
}

bool c_flag::operator== (const c_flag & right) const
{
	return f_value == right.f_value;
}

bool c_flag::operator!= (const c_flag & right) const
{
	return f_value != right.f_value;
}

c_flag::operator bool () const
{
	return f_value;
}





c_flag::t_value c_flag::get (void) const
{
	return f_value;
}

void c_flag::set (c_flag::t_value value)
{
	f_value = value;
}
