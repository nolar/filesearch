#ifndef _FILESEARCH_OBJECTS_PATH_
#define _FILESEARCH_OBJECTS_PATH_

#include <string>
#include <vector>
#include "c_object.h"

class c_path : public c_object
{
public:
	typedef std::string t_filename;
	typedef std::vector<t_filename> t_value;
private:
	t_value f_value;
protected:
public:
	c_path ();
	c_path (char *      value);
	c_path (std::string value);

	virtual bool stream_vary () const;
	virtual t_object_size stream_size () const;
	virtual void stream_getdata (      void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);

	bool operator== (const c_path & right) const;

	bool empty () const;
	void add (std::string value);
	c_path path_with_child (std::string child);

	std::string basename ();
	std::string dirname (bool heading_slash, bool empty_slash);
//	std::string ascii (bool heading_slash = true, bool empty_slash = true);
	std::string ascii (bool heading_slash, bool empty_slash);
};

#endif
