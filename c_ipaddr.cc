#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "c_ipaddr.h"

c_ipaddr::c_ipaddr ()
	: f_mask(32)
{
}

c_ipaddr::c_ipaddr (in_addr value, int mask)
{
	f_addr = value;
	f_mask = mask;
}

c_ipaddr::c_ipaddr (in_addr_t value, int mask)
{
	f_addr.s_addr = value;
	f_mask = mask;
}

c_ipaddr::c_ipaddr (char * value, int mask)
{
	int code = ascii2addr(AF_INET, value, &f_addr);
	if (code == -1) throw 1; //???!!!
	f_mask = mask;
}

c_ipaddr::c_ipaddr (std::string value, int mask)
{
	int code = ascii2addr(AF_INET, value.c_str(), &f_addr);
	if (code == -1) throw 1; //???!!!
	f_mask = mask;
}





t_object_size c_ipaddr::stream_size () const
{
	return sizeof(f_mask)+sizeof(f_addr);
}

void c_ipaddr::stream_getdata (void * buffer, t_object_size size) const
{
	char * buff = static_cast<char*>(buffer);
	memcpy(buff, &f_mask, sizeof(f_mask)); buff += sizeof(f_mask);
	memcpy(buff, &f_addr, sizeof(f_addr)); buff += sizeof(f_addr);
}

void c_ipaddr::stream_setdata (const void * buffer, t_object_size size)
{
	const char * buff = static_cast<const char *>(buffer);
	memcpy(&f_mask, buff, sizeof(f_mask)); buff += sizeof(f_mask);
	memcpy(&f_addr, buff, sizeof(f_addr)); buff += sizeof(f_addr);
}




bool c_ipaddr::operator== (const c_ipaddr & right) const
{
	return f_mask == right.f_mask
	    && f_addr.s_addr == right.f_addr.s_addr
	    ;
}





int c_ipaddr::mask ()
{
	return f_mask;
}

in_addr c_ipaddr::get_s ()
{
	return f_addr;
}

in_addr_t c_ipaddr::get_t ()
{
	return f_addr.s_addr;
}

std::string c_ipaddr::ascii ()
{
	char buffer[20];
	if (!addr2ascii(AF_INET, &f_addr, sizeof(f_addr), buffer))
		throw 1; //!!!
	return std::string(buffer);
}
