#include "c_identifier.h"
#include "globals.h"
#include <string.h>





c_identifier::c_identifier ()
	: c_object()
	, f_value()
{
}

c_identifier::c_identifier (const c_identifier & right)
	: c_object()
	, f_value(right.f_value)
{
}

c_identifier::c_identifier (c_identifier::t_value value)
	: c_object()
	, f_value(value)
{
}





bool c_identifier::stream_vary () const
{
	return false;
}

t_object_size c_identifier::stream_size () const
{
	return sizeof(f_value);
}

void c_identifier::stream_getdata (void * buffer, t_object_size size) const
{
	memcpy(buffer, &f_value, sizeof(f_value));
}

void c_identifier::stream_setdata (const void * buffer, t_object_size size)
{
	memcpy(&f_value, buffer, sizeof(f_value));
}





c_identifier & c_identifier::operator= (const c_identifier & right)
{
	f_value = right.f_value;
	return *this;
}

bool c_identifier::operator== (const c_identifier & right) const
{
	return f_value == right.f_value;
}

bool c_identifier::operator!= (const c_identifier & right) const
{
	return f_value != right.f_value;
}

bool c_identifier::operator<  (const c_identifier & right) const
{
	return f_value <  right.f_value;
}
bool c_identifier::operator<= (const c_identifier & right) const
{
	return f_value <= right.f_value;
}
bool c_identifier::operator>  (const c_identifier & right) const
{
	return f_value >  right.f_value;
}
bool c_identifier::operator>=  (const c_identifier & right) const
{
	return f_value >= right.f_value;
}





c_identifier::t_value c_identifier::get (void) const
{
	return f_value;
}

void c_identifier::set (c_identifier::t_value value)
{
	f_value = value;
}
