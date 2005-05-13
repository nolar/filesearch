#include <time.h>
#include <errno.h>
#include "typedefs.h"

const t_ipc_code ipc_code_ipaddr    = "ipaddr";
const t_ipc_code ipc_code_ipport    = "ipport";
const t_ipc_code ipc_code_root      = "root";
const t_ipc_code ipc_code_username  = "username";
const t_ipc_code ipc_code_password  = "password";
const t_ipc_code ipc_code_timeout   = "timeout";
const t_ipc_code ipc_code_depth     = "depth";
const t_ipc_code ipc_code_selfname  = "selfname";
const t_ipc_code ipc_code_workgroup = "workgroup";
const t_ipc_code ipc_code_bind      = "bind";


t_time::t_time (bool now)
{
	if (now)
	{
		time_t t1 = ::time(NULL);
		struct tm t;
		::gmtime_r(&t1, &t);
		if (errno) throw e_basic("Can not get current time.", errno, strerror(errno));
		f_year   = t.tm_year;
		f_month  = t.tm_mon ;
		f_day    = t.tm_mday;
		f_hour   = t.tm_hour;
		f_minute = t.tm_min ;
		f_second = t.tm_sec ;
	} else {
		f_year = f_month = f_day = f_hour = f_minute = f_second = 0;
	}
}

t_time::t_time (time_t value)
{
	struct tm t;
	::gmtime_r(&value, &t);
	if (errno) throw e_basic("Can not make time from specified time_t value.", errno, strerror(errno));
	f_year   = t.tm_year;
	f_month  = t.tm_mon ;
	f_day    = t.tm_mday;
	f_hour   = t.tm_hour;
	f_minute = t.tm_min ;
	f_second = t.tm_sec ;
}

t_time::t_time (int year, int month, int day, int hour, int minute, int second)
{
	f_year   = year  ;
	f_month  = month ;
	f_day    = day   ;
	f_hour   = hour  ;
	f_minute = minute;
	f_second = second;
}

t_time & t_time::operator= (time_t & right)
{
	struct tm t;
	::gmtime_r(&right, &t);
	if (errno) throw e_basic("Can not make time from specified time_t value.", errno, strerror(errno));
	f_year   = t.tm_year;
	f_month  = t.tm_mon ;
	f_day    = t.tm_mday;
	f_hour   = t.tm_hour;
	f_minute = t.tm_min ;
	f_second = t.tm_sec ;
	return *this;
}

t_time & t_time::operator= (t_time & right)
{
	f_year   = right.f_year  ;
	f_month  = right.f_month ;
	f_day    = right.f_day   ;
	f_hour   = right.f_hour  ;
	f_minute = right.f_minute;
	f_second = right.f_second;
	return *this;
}
