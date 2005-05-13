#ifndef _FILESEARCH_OBJECTS_SIGNED_
#define _FILESEARCH_OBJECTS_SIGNED_

#include "c_object.h"

class c_signed : public c_object
{
private:
	signed f_value;
protected:
public:
	c_signed ();
	c_signed (signed value);

	virtual t_object_size stream_size () const;
	virtual void stream_getdata (      void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);

	signed get ();
	void set (signed value);

	bool operator== (const c_signed & right) const;
	bool operator<  (const c_signed & right) const;
};

#endif
