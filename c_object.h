#ifndef _FILESEARCH_OBJECTS_OBJECT_
#define _FILESEARCH_OBJECTS_OBJECT_

typedef int t_object_size;

class c_object
{
private:
protected:
public:
	// constructors and destructor
	c_object ();
	virtual ~c_object ();

	// streaming routines
	virtual t_object_size stream_size () const = 0;
	virtual void stream_getdata (      void * buffer, t_object_size size) const = 0;
	virtual void stream_setdata (const void * buffer, t_object_size size) = 0;
};

#endif
