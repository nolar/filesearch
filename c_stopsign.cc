#include "c_stopsign.h"
#include "globals.h"
#include <string.h>





c_stopsign::c_stopsign ()
	: c_object()
{
}

c_stopsign::c_stopsign (const c_stopsign & right)
	: c_object()
{
}





bool c_stopsign::stream_vary () const
{
	return false;
}

t_object_size c_stopsign::stream_size () const
{
	return 0;
}

void c_stopsign::stream_getdata (void * buffer, t_object_size size) const
{
}

void c_stopsign::stream_setdata (const void * buffer, t_object_size size)
{
}
