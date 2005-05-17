#ifndef _FILESEARCH_OBJECTS_PATH_
#define _FILESEARCH_OBJECTS_PATH_

#include <string>
#include <vector>
#include "c_object.h"

class c_path : public c_object
{
private:
	std::vector<std::string> f_value;
protected:
public:
	c_path ();
	c_path (char *      value);
	c_path (std::string value);

	virtual t_object_size stream_size () const;
	virtual void stream_getdata (      void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);

	bool operator== (const c_path & right) const;

	bool empty () const;
	void add (std::string value);

	std::string basename ();
	std::string dirname (bool raw = false);
	std::string ascii (bool raw = false);
};

#endif
