#ifndef _FILESEARCH_OBJECTS_STOPSIGN_
#define _FILESEARCH_OBJECTS_STOPSIGN_

#include "c_object.h"

class c_stopsign : public c_object
{
private:
protected:
public:
	c_stopsign ();

	virtual t_object_size stream_size () const;
	virtual void stream_getdata (      void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);
};

#endif
