#ifndef _FILESEARCH_OBJECTS_PATH_
#define _FILESEARCH_OBJECTS_PATH_

#include "config.h"
#include "c_object.h"
#include "e_convert.h"
#include <string>
#include <vector>

class c_path : public c_object
{
public:
	typedef std::string t_filename;
	typedef std::vector<t_filename> t_value;
private:
	t_value f_value;
protected:
public:
	// constructors
	c_path ();
	c_path (const c_path & right);
	c_path (t_value value);
	c_path (t_value value, t_filename child);
	c_path (t_filename value, t_filename delimiter);
	c_path (t_filename value, t_filename delimiter, t_filename child);
	// streaming routines
	virtual bool stream_vary () const;
	virtual t_object_size stream_size () const;
	virtual void stream_getdata (void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);
	// operators
	c_path & operator= (const c_path & right);
	bool operator== (const c_path & right) const;
	bool operator!= (const c_path & right) const;
	c_path   operator+  (t_filename child) const;
	c_path & operator+= (t_filename child);
	// value handling routines
	bool empty () const;
	std::string basename () const;
	std::string dirname (bool heading_slash, bool empty_slash) const;
	std::string ascii (bool heading_slash, bool empty_slash) const;
};

#endif
