#ifndef _FILESEARCH_OBJECTS_STRING_
#define _FILESEARCH_OBJECTS_STRING_

#include "config.h"
#include "c_object.h"
#include "e_convert.h"
#include <string>

class c_string : public c_object
{
public:
	typedef std::string t_value;
	typedef std::string::size_type t_length;
private:
	t_value f_value;
protected:
public:
	// constructors
	c_string ();
	c_string (const c_string & right);
	c_string (t_value value);
	c_string (const char * value);
	// streaming routines
	virtual bool stream_vary () const;
	virtual t_object_size stream_size () const;
	virtual void stream_getdata (void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);
	// operators
	c_string & operator= (const c_string & right);
	bool operator== (const c_string & right) const;
	bool operator!= (const c_string & right) const;
	bool operator<  (const c_string & right) const;
	bool operator<= (const c_string & right) const;
	bool operator>  (const c_string & right) const;
	bool operator>= (const c_string & right) const;
	c_string operator+ (const c_string & right) const;
	// value handling routines
	t_value get (void) const;
	void set (t_value value);
	std::string ascii () const;
	const char * c_str () const;
	bool empty () const;
	t_length length () const;
};

#endif
