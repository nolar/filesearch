#include <string.h>
#include "c_stamp.h"

c_stamp::c_stamp ()
	: f_year()
	, f_month ()
	, f_day ()
	, f_hour ()
	, f_minute ()
	, f_second ()
{
}

c_stamp::c_stamp (int a_year, int a_month, int a_day, int a_hour, int a_minute, int a_second)
	: f_year(a_year)
	, f_month (a_month)
	, f_day (a_day)
	, f_hour (a_hour)
	, f_minute (a_minute)
	, f_second (a_second)
{
}

c_stamp::c_stamp (time_t value)
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





int c_stamp::year ()
{
	return f_year;
}

int c_stamp::month ()
{
	return f_month;
}

int c_stamp::day ()
{
	return f_day;
}

int c_stamp::hour ()
{
	return f_hour;
}

int c_stamp::minute ()
{
	return f_minute;
}

int c_stamp::second ()
{
	return f_second;
}

time_t c_stamp::unixtime ()
{
	struct tm t;
	t.tm_year = f_year - 1900;
	t.tm_mon  = f_month - 1;
	t.tm_mday = f_day;
	t.tm_hour = f_hour;
	t.tm_min  = f_minute;
	t.tm_sec  = f_second;
	time_t result = ::timegm(&t);
	if (result == -1) throw 1; //???!!!
	return result;
}
