#ifndef _FILESEARCH_OBJECTS_IDENTIFIER_
#define _FILESEARCH_OBJECTS_IDENTIFIER_

#include "config.h"
#include "c_object.h"
#include "e_convert.h"
#include <string>

class c_identifier : public c_object
{
public:
	typedef int t_value;
private:
	t_value f_value;
protected:
public:
	// constructors
	c_identifier ();
	c_identifier (const c_identifier & right);
	c_identifier (t_value value);
	// streaming routines
	virtual bool stream_vary () const;
	virtual t_object_size stream_size () const;
	virtual void stream_getdata (void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);
	// operators
	c_identifier & operator= (const c_identifier & right);
	bool operator== (const c_identifier & right) const;
	bool operator!= (const c_identifier & right) const;
	bool operator<  (const c_identifier & right) const;
	bool operator<= (const c_identifier & right) const;
	bool operator>  (const c_identifier & right) const;
	bool operator>= (const c_identifier & right) const;
	// value handling routines
	t_value get (void) const;
	void set (t_value value);
};

#endif
