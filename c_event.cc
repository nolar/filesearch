#include "c_event.h"
#include "globals.h"
#include <string.h>





const c_event::t_value c_event::value_resource = 1;
const c_event::t_value c_event::value_dir      = 2;
const c_event::t_value c_event::value_file     = 3;





c_event::c_event ()
	: c_object()
	, f_value()
{
}

c_event::c_event (const c_event & right)
	: c_object()
	, f_value(right.f_value)
{
}

c_event::c_event (c_event::t_value value)
	: c_object()
	, f_value(value)
{
}





bool c_event::stream_vary () const
{
	return false;
}

t_object_size c_event::stream_size () const
{
	return sizeof(f_value);
}

void c_event::stream_getdata (void * buffer, t_object_size size) const
{
	memcpy(buffer, &f_value, sizeof(f_value));
}

void c_event::stream_setdata (const void * buffer, t_object_size size)
{
	memcpy(&f_value, buffer, sizeof(f_value));
}





c_event & c_event::operator= (const c_event & right)
{
	f_value = right.f_value;
	return *this;
}

bool c_event::operator== (const c_event & right) const
{
	return f_value == right.f_value;
}

bool c_event::operator!= (const c_event & right) const
{
	return f_value != right.f_value;
}





c_event::t_value c_event::get (void) const
{
	return f_value;
}

void c_event::set (c_event::t_value value)
{
	f_value = value;
}

std::string c_event::ascii() const
{
	switch (f_value)
	{
		case  value_resource: return std::string("resource");
		case  value_dir     : return std::string("dir"     );
		case  value_file    : return std::string("file"    );
		default: return std::string("unknown") + "(" + string_format("%d", f_value) + ")";
	}
}





bool c_event::is_resource () const
{
	return f_value == value_resource;
}

bool c_event::is_dir () const
{
	return f_value == value_dir;
}

bool c_event::is_file () const
{
	return f_value == value_file;
}
