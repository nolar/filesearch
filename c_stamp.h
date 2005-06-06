#ifndef _FILESEARCH_OBJECTS_STAMP_
#define _FILESEARCH_OBJECTS_STAMP_

#include "config.h"
#include "c_object.h"
#include "e_convert.h"
#include <string>
#include <time.h>

class c_stamp : public c_object
{
public:
	typedef int t_year;
	typedef int t_month;
	typedef int t_day;
	typedef int t_hour;
	typedef int t_minute;
	typedef int t_second;
private:
	t_year   f_year;
	t_month  f_month;
	t_day    f_day;
	t_hour   f_hour;
	t_minute f_minute;
	t_second f_second;
protected:
public:
	// constructors
	c_stamp ();
	c_stamp (const c_stamp & right);
	c_stamp (t_year a_year, t_month a_month, t_day a_day);
	c_stamp (t_year a_year, t_month a_month, t_day a_day, t_hour a_hour, t_minute a_minute);
	c_stamp (t_year a_year, t_month a_month, t_day a_day, t_hour a_hour, t_minute a_minute, t_second a_second);
	c_stamp (time_t value);
	// streaming routines
	virtual bool stream_vary () const;
	virtual t_object_size stream_size () const;
	virtual void stream_getdata (void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);
	// operators
	c_stamp & operator= (const c_stamp & right);
	bool operator== (const c_stamp & right) const;
	bool operator!= (const c_stamp & right) const;
	bool operator<  (const c_stamp & right) const;
	bool operator<= (const c_stamp & right) const;
	bool operator>  (const c_stamp & right) const;
	bool operator>= (const c_stamp & right) const;
	// value handling routines
	t_year   get_year     () const; void set_year     (t_year   value);
	t_month  get_month    () const; void set_month    (t_month  value);
	t_day    get_day      () const; void set_day      (t_day    value);
	t_hour   get_hour     () const; void set_hour     (t_hour   value);
	t_minute get_minute   () const; void set_minute   (t_minute value);
	t_second get_second   () const; void set_second   (t_day    value);
	time_t   get_unixtime () const; void set_unixtime (time_t   value);
	std::string ascii (std::string format) const;
};

#endif
