#ifndef _FILESEARCH_OBJECTS_DOUBLE_
#define _FILESEARCH_OBJECTS_DOUBLE_

#include "config.h"
#include "c_object.h"
#include "e_convert.h"
#include <string>

class c_double : public c_object
{
public:
	typedef double t_value;
private:
	double f_value;
protected:
public:
	// constructors
	c_double ();
	c_double (const c_double & right);
	c_double (t_value value);
	// streaming
	virtual bool stream_vary () const;
	virtual t_object_size stream_size () const;
	virtual void stream_getdata (void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);
	// operators
	c_double & operator= (const c_double & right);
	bool operator== (const c_double & right) const;
	bool operator!= (const c_double & right) const;
	bool operator<  (const c_double & right) const;
	bool operator<= (const c_double & right) const;
	bool operator>  (const c_double & right) const;
	bool operator>= (const c_double & right) const;
	c_double operator* (const c_double & right) const;
	c_double operator/ (const c_double & right) const;
	c_double operator+ (const c_double & right) const;
	c_double operator- (const c_double & right) const;
	// value handling routines
	t_value get (void) const;
	void set (t_value value);
	std::string ascii () const;
};

#endif
