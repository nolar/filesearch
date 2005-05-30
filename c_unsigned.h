#ifndef _FILESEARCH_OBJECTS_UNSIGNED_
#define _FILESEARCH_OBJECTS_UNSIGNED_

#include <string>
#include "c_object.h"

class c_unsigned : public c_object
{
private:
	unsigned f_value;
protected:
public:
	c_unsigned ();
	c_unsigned (unsigned value);

	virtual bool stream_vary () const;
	virtual t_object_size stream_size () const;
	virtual void stream_getdata (      void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);

	unsigned get ();
	void set (unsigned value);
	std::string ascii (int pad = 0);

	c_unsigned & operator= (const c_unsigned & right);
	c_unsigned & operator= (const   unsigned   value);
	bool operator== (const c_unsigned & right) const;
	bool operator!= (const c_unsigned & right) const;
	bool operator<  (const c_unsigned & right) const;
	bool operator<= (const c_unsigned & right) const;
	bool operator>  (const c_unsigned & right) const;
	bool operator>= (const c_unsigned & right) const;
	c_unsigned operator* (const c_unsigned & right) const;
	c_unsigned operator* (const   unsigned   value) const;
	c_unsigned operator+ (const c_unsigned & right) const;
	c_unsigned operator+ (const   unsigned   value) const;
};

#endif
