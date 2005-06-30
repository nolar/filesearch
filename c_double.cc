#include "c_double.h"
#include "globals.h"
#include <string.h>





c_double::c_double ()
	: c_object()
	, f_value()
{
}

c_double::c_double (const c_double & right)
	: c_object()
	, f_value(right.f_value)
{
}

c_double::c_double (c_double::t_value value)
	: c_object()
	, f_value(value)
{
}





bool c_double::stream_vary () const
{
	return false;
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





c_double & c_double::operator= (const c_double & right)
{
	f_value = right.f_value;
	return *this;
}

bool c_double::operator== (const c_double & right) const
{
	return f_value == right.f_value;
}

bool c_double::operator!= (const c_double & right) const
{
	return f_value != right.f_value;
}

bool c_double::operator<  (const c_double & right) const
{
	return f_value < right.f_value;
}

bool c_double::operator<= (const c_double & right) const
{
	return f_value <= right.f_value;
}

bool c_double::operator>  (const c_double & right) const
{
	return f_value > right.f_value;
}

bool c_double::operator>= (const c_double & right) const
{
	return f_value >= right.f_value;
}

c_double c_double::operator* (const c_double & right) const
{
	return c_double(f_value * right.f_value);
}

c_double c_double::operator/ (const c_double & right) const
{
	return c_double(f_value / right.f_value);
}

c_double c_double::operator+ (const c_double & right) const
{
	return c_double(f_value + right.f_value);
}

c_double c_double::operator- (const c_double & right) const
{
	return c_double(f_value - right.f_value);
}





c_double::t_value c_double::get (void) const
{
	return f_value;
}

void c_double::set (c_double::t_value value)
{
	f_value = value;
}
