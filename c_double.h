#ifndef _FILESEARCH_OBJECTS_DOUBLE_
#define _FILESEARCH_OBJECTS_DOUBLE_

#include "c_object.h"

class c_double : public c_object
{
private:
	double f_value;
protected:
public:
	c_double ();
	c_double (double value);

	virtual t_object_size stream_size () const;
	virtual void stream_getdata (      void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);

	double get ();
	void set (double value);

	bool operator== (const c_double & right) const;
	bool operator<  (const c_double & right) const;
};

#endif
