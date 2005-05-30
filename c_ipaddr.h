#ifndef _FILESEARCH_OBJECTS_IPADDR_
#define _FILESEARCH_OBJECTS_IPADDR_

#include <netinet/in.h>
#include <sys/types.h>
#include <string>
#include "c_object.h"

class c_ipaddr : public c_object
{
public:
	typedef unsigned char t_mode;
	typedef unsigned char t_length;
	typedef unsigned char t_overflow;
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
	static const t_mode mode_none;
	static const t_mode mode_dns ;
	static const t_mode mode_ipv4;
	static const t_mode mode_ipv6;
	static const t_overflow no_overflow;
	static const t_overflow less_than_min;
	static const t_overflow more_than_max;
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
	c_ipaddr ();
	c_ipaddr (const c_ipaddr & right, bool make_concrete = false);
	explicit c_ipaddr (in_addr      value);
	explicit c_ipaddr (std::string  value);
	explicit c_ipaddr (in_addr      value, t_length length);
	explicit c_ipaddr (std::string  value, t_length length);

	virtual bool stream_vary () const;
	virtual t_object_size stream_size () const;
	virtual void stream_getdata (      void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);

	std::string ascii (bool force_mask = false) const;

	c_ipaddr first () const;
	c_ipaddr last  () const;
	c_ipaddr concrete () const;

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
};

#endif
