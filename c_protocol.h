#ifndef _FILESEARCH_OBJECTS_PROTOCOL_
#define _FILESEARCH_OBJECTS_PROTOCOL_

#include "c_object.h"

class c_protocol : public c_object
{
public:
	typedef unsigned char t_value;
private:
	t_value f_value;
protected:
public:
	c_protocol ();
	c_protocol (t_value value);

	virtual bool stream_vary () const;
	virtual t_object_size stream_size () const;
	virtual void stream_getdata (      void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);

	t_value get ();
	void set (t_value value);
	const char * ascii () const;

	bool operator== (const c_protocol & right) const;
};

#endif
