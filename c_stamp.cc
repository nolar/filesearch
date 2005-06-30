#include "c_stamp.h"
#include "globals.h"
#include <string.h>





c_stamp::c_stamp ()
	: c_object()
	, f_year()
	, f_month ()
	, f_day ()
	, f_hour ()
	, f_minute ()
	, f_second ()
{
}

c_stamp::c_stamp (const c_stamp & right)
	: c_object()
	, f_year(right.f_year)
	, f_month (right.f_month)
	, f_day (right.f_day)
	, f_hour (right.f_hour)
	, f_minute (right.f_minute)
	, f_second (right.f_second)
{
}

c_stamp::c_stamp (c_stamp::t_year a_year, c_stamp::t_month a_month, c_stamp::t_day a_day)
	: c_object()
	, f_year(a_year)
	, f_month (a_month)
	, f_day (a_day)
	, f_hour ()
	, f_minute ()
	, f_second ()
{
}

c_stamp::c_stamp (c_stamp::t_year a_year, c_stamp::t_month a_month, c_stamp::t_day a_day, c_stamp::t_hour a_hour, c_stamp::t_minute a_minute)
	: c_object()
	, f_year(a_year)
	, f_month (a_month)
	, f_day (a_day)
	, f_hour (a_hour)
	, f_minute (a_minute)
	, f_second ()
{
}

c_stamp::c_stamp (c_stamp::t_year a_year, c_stamp::t_month a_month, c_stamp::t_day a_day, c_stamp::t_hour a_hour, c_stamp::t_minute a_minute, c_stamp::t_second a_second)
	: c_object()
	, f_year(a_year)
	, f_month (a_month)
	, f_day (a_day)
	, f_hour (a_hour)
	, f_minute (a_minute)
	, f_second (a_second)
{
}

c_stamp::c_stamp (time_t value)
	: c_object()
{
	set_unixtime(value);
}





bool c_stamp::stream_vary () const
{
	return false;
}

t_object_size c_stamp::stream_size () const
{
	return sizeof(f_year)+sizeof(f_month)+sizeof(f_day)+sizeof(f_hour)+sizeof(f_minute)+sizeof(f_second);
}

void c_stamp::stream_getdata (void * buffer, t_object_size size) const
{
	char * buff = static_cast<char*>(buffer);
	memcpy(buff, &f_year  , sizeof(f_year  )); buff += sizeof(f_year  );
	memcpy(buff, &f_month , sizeof(f_month )); buff += sizeof(f_month );
	memcpy(buff, &f_day   , sizeof(f_day   )); buff += sizeof(f_day   );
	memcpy(buff, &f_hour  , sizeof(f_hour  )); buff += sizeof(f_hour  );
	memcpy(buff, &f_minute, sizeof(f_minute)); buff += sizeof(f_minute);
	memcpy(buff, &f_second, sizeof(f_second)); buff += sizeof(f_second);
}

void c_stamp::stream_setdata (const void * buffer, t_object_size size)
{
	const char * buff = static_cast<const char *>(buffer);
	memcpy(&f_year  , buff, sizeof(f_year  )); buff += sizeof(f_year  );
	memcpy(&f_month , buff, sizeof(f_month )); buff += sizeof(f_month );
	memcpy(&f_day   , buff, sizeof(f_day   )); buff += sizeof(f_day   );
	memcpy(&f_hour  , buff, sizeof(f_hour  )); buff += sizeof(f_hour  );
	memcpy(&f_minute, buff, sizeof(f_minute)); buff += sizeof(f_minute);
	memcpy(&f_second, buff, sizeof(f_second)); buff += sizeof(f_second);
}





c_stamp & c_stamp::operator= (const c_stamp & right)
{
	f_year   = right.f_year  ;
	f_month  = right.f_month ;
	f_day    = right.f_day   ;
	f_hour   = right.f_hour  ;
	f_minute = right.f_minute;
	f_second = right.f_second;
	return *this;
}

bool c_stamp::operator== (const c_stamp & right) const
{
	return f_year   == right.f_year
	    && f_month  == right.f_month
	    && f_day    == right.f_day
	    && f_hour   == right.f_hour
	    && f_minute == right.f_minute
	    && f_second == right.f_second
	    ;
}

bool c_stamp::operator!= (const c_stamp & right) const
{
	return f_year   != right.f_year
	    || f_month  != right.f_month
	    || f_day    != right.f_day
	    || f_hour   != right.f_hour
	    || f_minute != right.f_minute
	    || f_second != right.f_second
	    ;
}

bool c_stamp::operator<  (const c_stamp & right) const
{
	return (f_year  < right.f_year) ||
	       (f_year == right.f_year && f_month  < right.f_month ) ||
	       (f_year == right.f_year && f_month == right.f_month && f_day  < right.f_day) ||
	       (f_year == right.f_year && f_month == right.f_month && f_day == right.f_day && f_hour  < right.f_hour) ||
	       (f_year == right.f_year && f_month == right.f_month && f_day == right.f_day && f_hour == right.f_hour && f_minute  < right.f_minute) ||
	       (f_year == right.f_year && f_month == right.f_month && f_day == right.f_day && f_hour == right.f_hour && f_minute == right.f_minute && f_second  < right.f_second);
}

bool c_stamp::operator<= (const c_stamp & right) const
{
	return !operator>(right);
}

bool c_stamp::operator>  (const c_stamp & right) const
{
	return (f_year  > right.f_year) ||
	       (f_year == right.f_year && f_month  > right.f_month ) ||
	       (f_year == right.f_year && f_month == right.f_month && f_day  > right.f_day) ||
	       (f_year == right.f_year && f_month == right.f_month && f_day == right.f_day && f_hour  > right.f_hour) ||
	       (f_year == right.f_year && f_month == right.f_month && f_day == right.f_day && f_hour == right.f_hour && f_minute  > right.f_minute) ||
	       (f_year == right.f_year && f_month == right.f_month && f_day == right.f_day && f_hour == right.f_hour && f_minute == right.f_minute && f_second  > right.f_second);
}

bool c_stamp::operator>= (const c_stamp & right) const
{
	return !operator<(right);
}





c_stamp::t_year c_stamp::get_year () const
{
	return f_year;
}

c_stamp::t_month c_stamp::get_month () const
{
	return f_month;
}

c_stamp::t_day c_stamp::get_day () const
{
	return f_day;
}

c_stamp::t_hour c_stamp::get_hour () const
{
	return f_hour;
}

c_stamp::t_minute c_stamp::get_minute () const
{
	return f_minute;
}

c_stamp::t_second c_stamp::get_second () const
{
	return f_second;
}





void c_stamp::set_year (c_stamp::t_year value)
{
	f_year = value;
}

void c_stamp::set_month (c_stamp::t_month value)
{
	f_month = value;
}

void c_stamp::set_day (c_stamp::t_day value)
{
	f_day = value;
}

void c_stamp::set_hour (c_stamp::t_hour value)
{
	f_hour = value;
}

void c_stamp::set_minute (c_stamp::t_minute value)
{
	f_minute = value;
}

void c_stamp::set_second (c_stamp::t_second value)
{
	f_second = value;
}





time_t c_stamp::get_unixtime () const
{
	struct tm t;
	t.tm_year = f_year - 1900;
	t.tm_mon  = f_month - 1;
	t.tm_mday = f_day;
	t.tm_hour = f_hour;
	t.tm_min  = f_minute;
	t.tm_sec  = f_second;
	time_t result = ::timegm(&t);
	if (result == -1) throw e_basic(__FILE__,__LINE__,"Can not make unix timestamp"); //!!! make exception more detail, include code and y/m/d/h/m/s value to it
	return result;
}

void c_stamp::set_unixtime (time_t value)
{
	struct tm t;
	gmtime_r(&value, &t);
	f_year   = t.tm_year + 1900;
	f_month  = t.tm_mon + 1;
	f_day    = t.tm_mday;
	f_hour   = t.tm_hour;
	f_minute = t.tm_min;
	f_second = t.tm_sec;
}





std::string c_stamp::ascii (std::string format) const
{
	// !!! todo: make my own replacement of all %-operands in strftime with my values (because stamps out of unixtime range will not convert with strftime())
	return string_time(format.c_str(), get_unixtime());
}
