#ifndef _FILESEARCH_OBJECTS_STAMP_
#define _FILESEARCH_OBJECTS_STAMP_

#include <time.h>
#include "c_object.h"

class c_stamp : public c_object
{
private:
	int f_year;
	int f_month;
	int f_day;
	int f_hour;
	int f_minute;
	int f_second;
protected:
public:
	c_stamp ();
	c_stamp (int a_year, int a_month, int a_day, int a_hour = 0, int a_minute = 0, int a_second = 0);
	c_stamp (time_t value);

	virtual t_object_size stream_size () const;
	virtual void stream_getdata (      void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);

	int year ();
	int month ();
	int day ();
	int hour ();
	int minute ();
	int second ();
	time_t unixtime ();
	std::string ascii (std::string format);

	bool operator== (const c_stamp & right) const;

};

#endif
