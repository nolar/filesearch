#ifndef _FILESEARCH_OBJECTS_IPADDR_
#define _FILESEARCH_OBJECTS_IPADDR_

#include <netinet/in.h>
#include <sys/types.h>
#include <string>
#include "c_object.h"

class c_ipaddr : public c_object
{
private:
	in_addr f_addr;
	int f_mask;
protected:
public:
	c_ipaddr ();
	c_ipaddr (in_addr     value, int mask = 32);
	c_ipaddr (in_addr_t   value, int mask = 32);
	c_ipaddr (char *      value, int mask = 32);
	c_ipaddr (std::string value, int mask = 32);

	virtual t_object_size stream_size () const;
	virtual void stream_getdata (      void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);

	bool operator== (const c_ipaddr & right) const;

	int mask ();
	in_addr     get_s ();
	in_addr_t   get_t ();
	std::string ascii ();
};

#endif
