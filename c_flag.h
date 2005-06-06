#ifndef _FILESEARCH_OBJECTS_FLAG_
#define _FILESEARCH_OBJECTS_FLAG_

#include "config.h"
#include "c_object.h"
#include "e_convert.h"
#include <string>

class c_flag : public c_object
{
public:
	typedef bool t_value;
private:
	t_value f_value;
protected:
public:
	// constructors
	c_flag ();
	c_flag (const c_flag & right);
	c_flag (t_value value);
	// streaming routines
	virtual bool stream_vary () const;
	virtual t_object_size stream_size () const;
	virtual void stream_getdata (void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);
	//operators
	c_flag & operator= (const c_flag & right);
	bool operator== (const c_flag & right) const;
	bool operator!= (const c_flag & right) const;
	operator bool () const;
	// value handling routines
	t_value get (void) const;
	void set (t_value value);
	std::string ascii () const;
};

#endif
