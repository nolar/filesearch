#ifndef _FILESEARCH_OBJECTS_EVENT_
#define _FILESEARCH_OBJECTS_EVENT_

#include "config.h"
#include "c_object.h"
#include "e_convert.h"
#include <string>

class c_event : public c_object
{
public:
	typedef unsigned char t_value;
	static const t_value value_resource;
	static const t_value value_dir;
	static const t_value value_file;
private:
	t_value f_value;
protected:
public:
	// constructors
	c_event ();
	c_event (const c_event & right);
	c_event (t_value value);
	// streaming routines
	virtual bool stream_vary () const;
	virtual t_object_size stream_size () const;
	virtual void stream_getdata (void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);
	// operators
	c_event & operator= (const c_event & right);
	bool operator== (const c_event & right) const;
	bool operator!= (const c_event & right) const;
	// value handling routines
	t_value get (void) const;
	void set (t_value value);
	std::string ascii () const;
	// enumeration routines
	bool is_resource () const;
	bool is_dir      () const;
	bool is_file     () const;
};

#endif
