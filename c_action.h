#ifndef _FILESEARCH_OBJECTS_ACTION_
#define _FILESEARCH_OBJECTS_ACTION_

#include "c_object.h"

class c_action : public c_object
{
private:
	unsigned char f_value;
protected:
public:
	c_action ();
	c_action (unsigned char value);
	c_action (const c_action & right);

	virtual bool stream_vary () const;
	virtual t_object_size stream_size () const;
	virtual void stream_getdata (      void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);

	unsigned char get () const;
	void set (unsigned char value);

	bool operator== (const c_action & right) const;
};

#endif
