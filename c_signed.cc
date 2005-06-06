#include "c_signed.h"
#include "globals.h"
#include <string.h>





c_signed::c_signed ()
	: c_object()
	, f_value()
{
}

c_signed::c_signed (const c_signed & right)
	: c_object()
	, f_value(right.f_value)
{
}

c_signed::c_signed (c_signed::t_value value)
	: c_object()
	, f_value(value)
{
}





bool c_signed::stream_vary () const
{
	return false;
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





c_signed & c_signed::operator= (const c_signed & right)
{
	f_value = right.f_value;
	return *this;
}

bool c_signed::operator== (const c_signed & right) const
{
	return f_value == right.f_value;
}

bool c_signed::operator!= (const c_signed & right) const
{
	return f_value != right.f_value;
}

bool c_signed::operator<  (const c_signed & right) const
{
	return f_value < right.f_value;
}

bool c_signed::operator<= (const c_signed & right) const
{
	return f_value <= right.f_value;
}

bool c_signed::operator>  (const c_signed & right) const
{
	return f_value > right.f_value;
}

bool c_signed::operator>= (const c_signed & right) const
{
	return f_value >= right.f_value;
}

c_signed c_signed::operator* (const c_signed & right) const
{
	return c_signed(f_value * right.f_value);
}

c_signed c_signed::operator/ (const c_signed & right) const
{
	return c_signed(f_value / right.f_value);
}

c_signed c_signed::operator% (const c_signed & right) const
{
	return c_signed(f_value % right.f_value);
}

c_signed c_signed::operator+ (const c_signed & right) const
{
	return c_signed(f_value + right.f_value);
}

c_signed c_signed::operator- (const c_signed & right) const
{
	return c_signed(f_value - right.f_value);
}





c_signed::t_value c_signed::get (void) const
{
	return f_value;
}

void c_signed::set (c_signed::t_value value)
{
	f_value = value;
}

std::string c_signed::ascii (int pad) const
{
	return string_format("%*d", pad, f_value); 
}
