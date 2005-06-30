#include "c_unsigned.h"
#include "globals.h"
#include <string.h>





c_unsigned::c_unsigned ()
	: c_object()
	, f_value()
{
}

c_unsigned::c_unsigned (const c_unsigned & right)
	: c_object()
	, f_value(right.f_value)
{
}

c_unsigned::c_unsigned (c_unsigned::t_value value)
	: c_object()
	, f_value(value)
{
}





bool c_unsigned::stream_vary () const
{
	return false;
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





c_unsigned & c_unsigned::operator= (const c_unsigned & right)
{
	f_value = right.f_value;
	return *this;
}

bool c_unsigned::operator== (const c_unsigned & right) const
{
	return f_value == right.f_value;
}

bool c_unsigned::operator!= (const c_unsigned & right) const
{
	return f_value != right.f_value;
}

bool c_unsigned::operator<  (const c_unsigned & right) const
{
	return f_value < right.f_value;
}

bool c_unsigned::operator<= (const c_unsigned & right) const
{
	return f_value <= right.f_value;
}

bool c_unsigned::operator>  (const c_unsigned & right) const
{
	return f_value > right.f_value;
}

bool c_unsigned::operator>= (const c_unsigned & right) const
{
	return f_value >= right.f_value;
}

c_unsigned c_unsigned::operator* (const c_unsigned & right) const
{
	return c_unsigned(f_value * right.f_value);
}

c_unsigned c_unsigned::operator/ (const c_unsigned & right) const
{
	return c_unsigned(f_value / right.f_value);
}

c_unsigned c_unsigned::operator% (const c_unsigned & right) const
{
	return c_unsigned(f_value % right.f_value);
}

c_unsigned c_unsigned::operator+ (const c_unsigned & right) const
{
	return c_unsigned(f_value + right.f_value);
}

c_unsigned c_unsigned::operator- (const c_unsigned & right) const
{
	return c_unsigned(f_value - right.f_value);
}





c_unsigned::t_value c_unsigned::get (void) const
{
	return f_value;
}

void c_unsigned::set (c_unsigned::t_value value)
{
	f_value = value;
}

std::string c_unsigned::ascii (int pad) const
{
	return string_format("%*u", pad, f_value);
}
