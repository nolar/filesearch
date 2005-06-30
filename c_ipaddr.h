#ifndef _FILESEARCH_OBJECTS_IPADDR_
#define _FILESEARCH_OBJECTS_IPADDR_

#include "config.h"
#include "c_object.h"
#include "e_convert.h"
#include <string>
#include <netinet/in.h>
#include <sys/types.h>

class c_ipaddr : public c_object
{
public:
	typedef unsigned char t_mode;
	typedef unsigned char t_length;
	typedef unsigned char t_overflow;
	static const t_mode mode_none;
	static const t_mode mode_dns ;
	static const t_mode mode_ipv4;
	static const t_mode mode_ipv6;
	static const t_overflow no_overflow;
	static const t_overflow less_than_min;
	static const t_overflow more_than_max;
private:
	t_mode f_mode;
	t_overflow f_overflow;
	union {
		struct {
			in_addr  addr;
			t_length length;
		} ipv4;
	} f_data;
protected:
	// supplimentary routines for IP version 4
	static in_addr ipv4_min   ();
	static in_addr ipv4_max   ();
	static in_addr ipv4_mask  (t_length length);
	static in_addr ipv4_net   (in_addr addr, in_addr mask);
	static in_addr ipv4_host  (in_addr addr, in_addr mask);
	static in_addr ipv4_addr  (in_addr net, in_addr host);
	static in_addr ipv4_addr  (in_addr net, in_addr host, in_addr mask);
	static bool    ipv4_ismin (in_addr addr);
	static bool    ipv4_ismax (in_addr addr);
	static bool    ipv4_cmpeq (in_addr addr1, in_addr addr2);
	static bool    ipv4_cmpls (in_addr addr1, in_addr addr2);
	static bool    ipv4_cmpgt (in_addr addr1, in_addr addr2);
public:
	// constructors
	c_ipaddr ();
	c_ipaddr (const c_ipaddr & right);
	c_ipaddr (in_addr      value);
	c_ipaddr (std::string  value);
	c_ipaddr (in_addr      value, t_length length);
	c_ipaddr (std::string  value, t_length length);
	// streaming routines
	virtual bool stream_vary () const;
	virtual t_object_size stream_size () const;
	virtual void stream_getdata (void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);
	// operators
	c_ipaddr & operator= (const c_ipaddr & right);
	bool operator== (const c_ipaddr & right) const;
	bool operator!= (const c_ipaddr & right) const;
	bool operator<  (const c_ipaddr & right) const;
	bool operator<= (const c_ipaddr & right) const;
	bool operator>  (const c_ipaddr & right) const;
	bool operator>= (const c_ipaddr & right) const;
	c_ipaddr & operator++ ();
	c_ipaddr & operator++ (int);
	c_ipaddr & operator-- ();
	c_ipaddr & operator-- (int);
	// value handling routines
	c_ipaddr range_first () const;
	c_ipaddr range_last  () const;
	c_ipaddr concrete    () const;
	std::string ascii (signed mask_mode = 0) const;
};

#endif
