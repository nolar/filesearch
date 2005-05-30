#ifndef _FILESEARCH_OBJECTS_STRING_
#define _FILESEARCH_OBJECTS_STRING_

#include <string>
#include "c_object.h"

class c_string : public c_object
{
private:
	std::string f_value;
protected:
public:
	c_string ();
	c_string (char * value);
	c_string (std::string value);

	virtual bool stream_vary () const;
	virtual t_object_size stream_size () const;
	virtual void stream_getdata (      void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);

	std::string get () const;
	void set (std::string value);
	const char * c_str () const;
	bool empty () const;

	c_string & operator= (const c_string &   right);
	c_string & operator= (const std::string  value);
	c_string & operator= (const char *       value);
	bool operator== (const c_string & right) const;
	bool operator!= (const c_string & right) const;
	bool operator<  (const c_string & right) const;
	bool operator<= (const c_string & right) const;
	bool operator>  (const c_string & right) const;
	bool operator>= (const c_string & right) const;
	c_string operator+ (const c_string &    right) const;
	c_string operator+ (const std::string & value) const;
	c_string operator+ (const char *        value) const;
};

#endif
