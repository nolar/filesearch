#ifndef _FILESEARCH_OBJECTS_MAPKEY_
#define _FILESEARCH_OBJECTS_MAPKEY_

#include "c_object.h"

class c_mapkey : public c_object
{
private:
	int f_value;
protected:
public:
	c_mapkey ();
	c_mapkey (int value);

	virtual t_object_size stream_size () const;
	virtual void stream_getdata (      void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);

	int get () const;
	void set (int value);

	bool operator== (const c_mapkey & right) const;
	bool operator<  (const c_mapkey & right) const;
};

#endif
