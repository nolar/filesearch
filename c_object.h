#ifndef _FILESEARCH_OBJECTS_OBJECT_
#define _FILESEARCH_OBJECTS_OBJECT_

#include "config.h"
#include "e_basic.h"

typedef int t_object_size;

class c_object
{
private:
	unsigned long f_references;
protected:
public:
	// constructors and destructor
	c_object ();
	virtual ~c_object ();
	// streaming routines
	virtual bool stream_vary () const = 0;
	virtual t_object_size stream_size () const = 0;
	virtual void stream_getdata (      void * buffer, t_object_size size) const = 0;
	virtual void stream_setdata (const void * buffer, t_object_size size) = 0;
	// referencing routines (for auto-deallocating in streambox)
	void reference_increase ();
	void reference_decrease ();
	bool reference_zero () const;
};

#endif
