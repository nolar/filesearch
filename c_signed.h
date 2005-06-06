#ifndef _FILESEARCH_OBJECTS_SIGNED_
#define _FILESEARCH_OBJECTS_SIGNED_

#include "config.h"
#include "c_object.h"
#include "e_convert.h"
#include <string>

class c_signed : public c_object
{
public:
	typedef signed t_value;
private:
	t_value f_value;
protected:
public:
	// constructors
	c_signed ();
	c_signed (const c_signed & right);
	c_signed (t_value value);
	// streaming routines
	virtual bool stream_vary () const;
	virtual t_object_size stream_size () const;
	virtual void stream_getdata (void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);
	// operators
	c_signed & operator= (const c_signed & right);
	bool operator== (const c_signed & right) const;
	bool operator!= (const c_signed & right) const;
	bool operator<  (const c_signed & right) const;
	bool operator<= (const c_signed & right) const;
	bool operator>  (const c_signed & right) const;
	bool operator>= (const c_signed & right) const;
	c_signed operator* (const c_signed & right) const;
	c_signed operator/ (const c_signed & right) const;
	c_signed operator% (const c_signed & right) const;
	c_signed operator+ (const c_signed & right) const;
	c_signed operator- (const c_signed & right) const;
	// value handling routines
	t_value get (void) const;
	void set (t_value value);
	std::string ascii (int pad = 0) const;
};

#endif
