#ifndef _FILESEARCH_OBJECTS_STOPSIGN_
#define _FILESEARCH_OBJECTS_STOPSIGN_

#include "config.h"
#include "c_object.h"
#include "e_convert.h"
#include <string>

class c_stopsign : public c_object
{
private:
protected:
public:
	// constructors
	c_stopsign ();
	c_stopsign (const c_stopsign & right);
	// streaming routines
	virtual bool stream_vary () const;
	virtual t_object_size stream_size () const;
	virtual void stream_getdata (void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);
};

#endif
