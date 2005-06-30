#ifndef _FILESEARCH_OBJECTS_UNSIGNED_
#define _FILESEARCH_OBJECTS_UNSIGNED_

#include "config.h"
#include "c_object.h"
#include "e_convert.h"
#include <string>

class c_unsigned : public c_object
{
public:
	typedef unsigned t_value;
private:
	t_value f_value;
protected:
public:
	// constructors
	c_unsigned ();
	c_unsigned (const c_unsigned & right);
	c_unsigned (t_value value);
	// streaming routines
	virtual bool stream_vary () const;
	virtual t_object_size stream_size () const;
	virtual void stream_getdata (void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);
	// operators
	c_unsigned & operator= (const c_unsigned & right);
	bool operator== (const c_unsigned & right) const;
	bool operator!= (const c_unsigned & right) const;
	bool operator<  (const c_unsigned & right) const;
	bool operator<= (const c_unsigned & right) const;
	bool operator>  (const c_unsigned & right) const;
	bool operator>= (const c_unsigned & right) const;
	c_unsigned operator* (const c_unsigned & right) const;
	c_unsigned operator/ (const c_unsigned & right) const;
	c_unsigned operator% (const c_unsigned & right) const;
	c_unsigned operator+ (const c_unsigned & right) const;
	c_unsigned operator- (const c_unsigned & right) const;
	// value handling routines
	t_value get (void) const;
	void set (t_value value);
	std::string ascii (int pad = 0) const;
};

#endif
