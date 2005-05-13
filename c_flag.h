#ifndef _FILESEARCH_OBJECTS_FLAG_
#define _FILESEARCH_OBJECTS_FLAG_

#include "c_object.h"

class c_flag : public c_object
{
private:
	bool f_value;
protected:
public:
	c_flag ();
	c_flag (bool value);

	virtual t_object_size stream_size () const;
	virtual void stream_getdata (      void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);

	bool get ();
	void set (bool value);

	bool operator== (const c_flag & right) const;
	bool operator<  (const c_flag & right) const;
};

#endif
