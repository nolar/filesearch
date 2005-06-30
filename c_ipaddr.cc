#include "c_ipaddr.h"
#include "globals.h"
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>





const c_ipaddr::t_mode     c_ipaddr::mode_none     = 0;
const c_ipaddr::t_mode     c_ipaddr::mode_dns      = 1;
const c_ipaddr::t_mode     c_ipaddr::mode_ipv4     = 2;
const c_ipaddr::t_mode     c_ipaddr::mode_ipv6     = 3;

const c_ipaddr::t_overflow c_ipaddr::no_overflow   = 0;
const c_ipaddr::t_overflow c_ipaddr::less_than_min = 1;
const c_ipaddr::t_overflow c_ipaddr::more_than_max = 2;





c_ipaddr::c_ipaddr ()
	: c_object()
	, f_mode(mode_none)
	, f_overflow(no_overflow)
{
}

c_ipaddr::c_ipaddr (const c_ipaddr & right)
	: c_object()
	, f_mode(right.f_mode)
	, f_overflow(right.f_overflow)
	, f_data(right.f_data)
{
}

c_ipaddr::c_ipaddr (in_addr value)
	: c_object()
	, f_mode(mode_ipv4)
	, f_overflow(no_overflow)
{
	f_data.ipv4.length = 32;
	f_data.ipv4.addr = value;
}

c_ipaddr::c_ipaddr (std::string value)
	: c_object()
	, f_overflow(no_overflow)
{
	int code;
	in_addr addr4;
	if ((code = ascii2addr(AF_INET, value.c_str(), &addr4)) != -1)
	{
		f_mode = mode_ipv4;
		f_data.ipv4.length = 32;
		f_data.ipv4.addr = addr4;
	} else
//	if ((code = ascii2addr6(.....)) != -1)
//	{
//		f_mode = mode_ipv6;
//		...
//	} else
	{
		f_mode = mode_none;
	}
}


c_ipaddr::c_ipaddr (in_addr value, c_ipaddr::t_length length)
	: c_object()
	, f_mode(mode_ipv4)
	, f_overflow(no_overflow)
{
	f_data.ipv4.length = length;
	f_data.ipv4.addr = value;
}

c_ipaddr::c_ipaddr (std::string value, c_ipaddr::t_length length)
	: c_object()
	, f_overflow(no_overflow)
{
	int code;
	in_addr addr4;
	if ((code = ascii2addr(AF_INET, value.c_str(), &addr4)) != -1)
	{
		f_mode = mode_ipv4;
		f_data.ipv4.length = length;
		f_data.ipv4.addr = addr4;
	} else
//	if ((code = ascii2addr6(.....)) != -1)
//	{
//		f_mode = mode_ipv6;
//		...
//	} else
	{
		f_mode = mode_none;
	}
}





bool c_ipaddr::stream_vary () const
{
	return false;
}

t_object_size c_ipaddr::stream_size () const
{
	return sizeof(f_mode)+sizeof(f_overflow)+sizeof(f_data);
}

void c_ipaddr::stream_getdata (void * buffer, t_object_size size) const
{
	char * buff = static_cast<char*>(buffer);
	memcpy(buff, &f_mode    , sizeof(f_mode    )); buff += sizeof(f_mode    );
	memcpy(buff, &f_overflow, sizeof(f_overflow)); buff += sizeof(f_overflow);
	memcpy(buff, &f_data    , sizeof(f_data    )); buff += sizeof(f_data    );
}

void c_ipaddr::stream_setdata (const void * buffer, t_object_size size)
{
	const char * buff = static_cast<const char *>(buffer);
	memcpy(&f_mode    , buff, sizeof(f_mode    )); buff += sizeof(f_mode    );
	memcpy(&f_overflow, buff, sizeof(f_overflow)); buff += sizeof(f_overflow);
	memcpy(&f_data    , buff, sizeof(f_data    )); buff += sizeof(f_data    );
}





c_ipaddr & c_ipaddr::operator= (const c_ipaddr & right)
{
	f_mode = right.f_mode;
	f_overflow = right.f_overflow;
	f_data = right.f_data;
	return * this;
}

bool c_ipaddr::operator== (const c_ipaddr & right) const
{
	bool result = (f_mode == right.f_mode) && (f_overflow == right.f_overflow);
	if (result) if (f_overflow == no_overflow) switch (f_mode)
	{
		case mode_ipv4:
			result = ipv4_cmpeq(f_data.ipv4.addr, right.f_data.ipv4.addr);
			break;
//		case mode_ipv6:
//			result = cmpeq6(f_data.ipv6.addr, right.f_data.ipv6.addr);
//			break;
		default:
			break;
	}
	return result;
}

bool c_ipaddr::operator!= (const c_ipaddr & right) const
{
	bool result = (f_mode != right.f_mode) || (f_overflow != right.f_overflow);
	if (!result) if (f_overflow == no_overflow) switch (f_mode)
	{
		case mode_ipv4:
			result = !ipv4_cmpeq(f_data.ipv4.addr, right.f_data.ipv4.addr);
			break;
//		case mode_ipv6:
//			...
//			break;
		default:
			break;
	}
	return result;
}

bool c_ipaddr::operator< (const c_ipaddr & right) const
{
	bool result = (f_mode == right.f_mode);
	if (result)
	switch (f_overflow)
	{
		case less_than_min:
			result = (right.f_overflow != less_than_min);
			break;
		case more_than_max:
			result = false;
			break;
		case no_overflow:
			switch (f_mode)
			{
				case mode_ipv4:
					result = ipv4_cmpls(f_data.ipv4.addr, right.f_data.ipv4.addr);
					break;
//				case mode_ipv6:
//					...
//					break;
				default:
					break;
			}
			break;
	}
	return result;
}

bool c_ipaddr::operator<= (const c_ipaddr & right) const
{
	bool result = (f_mode == right.f_mode);
	if (result)
	switch (f_overflow)
	{
		case less_than_min:
			result = true;
			break;
		case more_than_max:
			result = (right.f_overflow == more_than_max);
			break;
		case no_overflow:
			switch (f_mode)
			{
				case mode_ipv4:
					result = ipv4_cmpls(f_data.ipv4.addr, right.f_data.ipv4.addr) || ipv4_cmpeq(f_data.ipv4.addr, right.f_data.ipv4.addr);
					break;
//				case mode_ipv6:
//					...
//					break;
				default:
					break;
			}
			break;
	}
	return result;
}

bool c_ipaddr::operator> (const c_ipaddr & right) const
{
	bool result = (f_mode == right.f_mode);
	if (result)
	switch (f_overflow)
	{
		case less_than_min:
			result = false;
			break;
		case more_than_max:
			result = (right.f_overflow != more_than_max);
			break;
		case no_overflow:
			switch (f_mode)
			{
				case mode_ipv4:
					result = ipv4_cmpgt(f_data.ipv4.addr, right.f_data.ipv4.addr);
					break;
//				case mode_ipv6:
//					...
//					break;
				default:
					break;
			}
			break;
	}
	return result;
}

bool c_ipaddr::operator>= (const c_ipaddr & right) const
{
	bool result = (f_mode == right.f_mode);
	if (result)
	switch (f_overflow)
	{
		case less_than_min:
			result = (right.f_overflow == less_than_min);
			break;
		case more_than_max:
			result = true;
			break;
		case no_overflow:
			switch (f_mode)
			{
				case mode_ipv4:
					result = ipv4_cmpgt(f_data.ipv4.addr, right.f_data.ipv4.addr) || ipv4_cmpeq(f_data.ipv4.addr, right.f_data.ipv4.addr);
					break;
//				case mode_ipv6:
//					...
//					break;
				default:
					break;
			}
			break;
	}
	return result;
}

c_ipaddr & c_ipaddr::operator++ ()
{
	switch (f_mode)
	{
		case mode_ipv4:
			if (f_overflow == less_than_min)
				{ f_data.ipv4.addr = ipv4_min(); f_overflow = no_overflow; } else
			if (f_overflow == no_overflow  ) 
				if (ipv4_ismax(f_data.ipv4.addr)) f_overflow = more_than_max; else
				f_data.ipv4.addr.s_addr = htonl(ntohl(f_data.ipv4.addr.s_addr)+1);
			break;
		default:
			// throw???
			break;
	}
	return *this;
}

c_ipaddr & c_ipaddr::operator++ (int)
{
	switch (f_mode)
	{
		case mode_ipv4:
			if (f_overflow == less_than_min)
				{ f_data.ipv4.addr = ipv4_min(); f_overflow = no_overflow; } else
			if (f_overflow == no_overflow  ) 
				if (ipv4_ismax(f_data.ipv4.addr)) f_overflow = more_than_max; else
				f_data.ipv4.addr.s_addr = htonl(ntohl(f_data.ipv4.addr.s_addr)+1);
			break;
		default:
			// throw???
			break;
	}
	return *this;
}

c_ipaddr & c_ipaddr::operator-- ()
{
	switch (f_mode)
	{
		case mode_ipv4:
			if (f_overflow == more_than_max)
				{ f_data.ipv4.addr = ipv4_max(); f_overflow = no_overflow; } else
			if (f_overflow == no_overflow  )
				if (ipv4_ismin(f_data.ipv4.addr)) f_overflow = less_than_min; else
				f_data.ipv4.addr.s_addr = htonl(ntohl(f_data.ipv4.addr.s_addr)-1);
			break;
		default:
			// throw???
			break;
	}
	return *this;
}

c_ipaddr & c_ipaddr::operator-- (int)
{
	switch (f_mode)
	{
		case mode_ipv4:
			if (f_overflow == more_than_max)
				{ f_data.ipv4.addr = ipv4_max(); f_overflow = no_overflow; } else
			if (f_overflow == no_overflow  )
				if (ipv4_ismin(f_data.ipv4.addr)) f_overflow = less_than_min; else
				f_data.ipv4.addr.s_addr = htonl(ntohl(f_data.ipv4.addr.s_addr)-1);
			break;
		default:
			// throw???
			break;
	}
	return *this;
}





c_ipaddr c_ipaddr::range_first () const
{
	c_ipaddr result;
	result.f_mode = f_mode;
	switch (f_mode)
	{
		case mode_ipv4:
			result.f_data.ipv4.length = f_data.ipv4.length;
			result.f_data.ipv4.addr = ipv4_addr(f_data.ipv4.addr, ipv4_min(), ipv4_mask(f_data.ipv4.length));
			break;
		default:
			// throw???
			break;
	}
	return result;
}

c_ipaddr c_ipaddr::range_last () const
{
	c_ipaddr result;
	result.f_mode = f_mode;
	switch (f_mode)
	{
		case mode_ipv4:
			result.f_data.ipv4.length = f_data.ipv4.length;
			result.f_data.ipv4.addr = ipv4_addr(f_data.ipv4.addr, ipv4_max(), ipv4_mask(f_data.ipv4.length));
			break;
		default:
			// throw???
			break;
	}
	return result;
}

c_ipaddr c_ipaddr::concrete () const
{
	c_ipaddr result;
	result.f_mode = f_mode;
	result.f_overflow = f_overflow;
	result.f_data = f_data;
	switch (f_mode)
	{
		case mode_ipv4:
			result.f_data.ipv4.length = 32;
			break;
		default:
			// throw???
			break;
	}
	return result;
}

std::string c_ipaddr::ascii (signed mask_mode) const
{
	std::string result;
	std::auto_ptr<char> buffer(new char[20]);
	switch (f_overflow)
	{
		case more_than_max:
			result = "more-than-max";
			break;
		case less_than_min:
			result = "less-than-min";
			break;
		case no_overflow:
			switch (f_mode)
			{
				case mode_ipv4:
					if (!addr2ascii(AF_INET, &f_data.ipv4.addr, sizeof(f_data.ipv4.addr), buffer.get()))
						throw e_convert(__FILE__,__LINE__,"Can not convert IP address to string.");
					result = buffer.get();
					if ((mask_mode > 0) || (mask_mode == 0 && f_data.ipv4.length != 32))
						result = result + "/" + string_format("%d", f_data.ipv4.length);
					break;
				default:
					result = "unknown"; //??? throw here?
			}
			break;
	}
	return result;
}





in_addr c_ipaddr::ipv4_min ()
{
	in_addr result;
	memset(&result, 0, sizeof(result));
	return result;
}

in_addr c_ipaddr::ipv4_max ()
{
	in_addr result;
	memset(&result, 0xFF, sizeof(result));
	return result;
}

in_addr c_ipaddr::ipv4_mask (c_ipaddr::t_length length)
{
	in_addr result;
	result.s_addr = htonl(0xFFFFFFFF << (32 - length));
	return result;
}

in_addr c_ipaddr::ipv4_net (in_addr addr, in_addr mask)
{
	in_addr result;
	result.s_addr = addr.s_addr & mask.s_addr;
	return result;
}

in_addr c_ipaddr::ipv4_host (in_addr addr, in_addr mask)
{
	in_addr result;
	result.s_addr = addr.s_addr & ~mask.s_addr;
	return result;
}

in_addr c_ipaddr::ipv4_addr (in_addr net, in_addr host)
{
	in_addr result;
	result.s_addr = net.s_addr | host.s_addr;
	return result;
}

in_addr c_ipaddr::ipv4_addr (in_addr net, in_addr host, in_addr mask)
{
	in_addr result;
	result.s_addr = (net.s_addr & mask.s_addr) | (host.s_addr & ~mask.s_addr);
	return result;
}

bool c_ipaddr::ipv4_ismin (in_addr addr)
{
	return (addr.s_addr == 0);
}

bool c_ipaddr::ipv4_ismax (in_addr addr)
{
	return (addr.s_addr == 0xFFFFFFFF);
}

bool c_ipaddr::ipv4_cmpeq (in_addr addr1, in_addr addr2)
{
	return addr1.s_addr == addr2.s_addr;
}

bool c_ipaddr::ipv4_cmpls (in_addr addr1, in_addr addr2)
{
	return ntohl(addr1.s_addr) < ntohl(addr2.s_addr);
}

bool c_ipaddr::ipv4_cmpgt (in_addr addr1, in_addr addr2)
{
	return ntohl(addr1.s_addr) > ntohl(addr2.s_addr);
}
