#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdarg.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "convert.h"



const int convert::_buffer_size = 1024;
const char * convert::_time_format = "%Y-%m-%d %H:%M:%S";


std::string convert::_sprintf (const char * format, ...)
{
	va_list ap; va_start(ap, format);
	std::string::value_type * buffer = new std::string::value_type [_buffer_size];
	int code = ::vsnprintf(buffer, _buffer_size, format, ap);
	std::string result((code < 0) ? "" : buffer);
	delete[] buffer;
	va_end(ap);
	if (code < 0) throw e_convert(std::string()+"Can not format string to '"+format+"'.", errno, strerror(errno));
	return result;
}

std::string convert::_strftime (const char * format, time_t value)
{
	std::string::value_type * buffer = new std::string::value_type [_buffer_size];
	int code = ::strftime(buffer, _buffer_size, (format && *format)?format:_time_format, localtime(&value));
	std::string result((code < 0) ? "" : buffer);
	delete[] buffer;
	if (code < 0) throw e_convert(std::string()+"Can not format timestamp to '"+_time_format+"'.", errno, strerror(errno));
	return result;
	
}

std::vector<std::string> convert::_split (std::string value, std::string delimiter)
{
	std::vector<std::string> result;
	std::string::size_type delimlen = delimiter.length(), idx, pos, prev = (std::string::size_type) -delimlen;
	do {
		idx = (prev+delimlen >= value.length()) ? std::string::npos : value.find(delimiter, prev+delimlen);
		pos = (idx == std::string::npos) ? value.length() : idx;
		std::string item = value.substr(prev+delimlen, pos-(prev+delimlen));
		if (!item.empty()) result.push_back(item);
		prev = pos;
	} while (idx != std::string::npos);
	return result;
}



unsigned int convert::str2ui (std::string value)
{
	if (value.empty()) throw e_convert("Can not convert empty string to unsigned int.");
	std::string::value_type * endptr;
	unsigned int result = (unsigned int) ::strtoul(value.c_str(), &endptr, 10);
	if (*endptr) throw e_convert(std::string()+"Can not convert string '"+value+"' to unsigned int.", errno, strerror(errno));
	return result;
}

signed int convert::str2si (std::string value)
{
	if (value.empty()) throw e_convert("Can not convert empty string to signed int.");
	std::string::value_type * endptr;
	signed int result = (signed int) ::strtol(value.c_str(), &endptr, 10);
	if (*endptr) throw e_convert(std::string()+"Can not convert string '"+value+"' to signed int.", errno, strerror(errno));
	return result;
}

unsigned long convert::str2ul (std::string value)
{
	if (value.empty()) throw e_convert("Can not convert empty string to unsigned long.");
	std::string::value_type * endptr;
	unsigned long result = ::strtoul(value.c_str(), &endptr, 10);
	if (*endptr) throw e_convert(std::string()+"Can not convert string '"+value+"' to unsigned long.", errno, strerror(errno));
	return result;
}

signed long convert::str2sl (std::string value)
{
	if (value.empty()) throw e_convert("Can not convert empty string to signed long.");
	std::string::value_type * endptr;
	signed long result = ::strtol(value.c_str(), &endptr, 10);
	if (*endptr) throw e_convert(std::string()+"Can not convert string '"+value+"' to signed long.", errno, strerror(errno));
	return result;
}

unsigned long long convert::str2ull (std::string value)
{
	if (value.empty()) throw e_convert("Can not convert empty string to unsigned long long.");
	std::string::value_type * endptr;
	unsigned long long result = ::strtoull(value.c_str(), &endptr, 10);
	if (*endptr) throw e_convert(std::string()+"Can not convert string '"+value+"' to unsigned long long.", errno, strerror(errno));
	return result;
}

signed long long convert::str2sll (std::string value)
{
	if (value.empty()) throw e_convert("Can not convert empty string to signed long long.");
	std::string::value_type * endptr;
	signed long long result = ::strtoll(value.c_str(), &endptr, 10);
	if (*endptr) throw e_convert(std::string()+"Can not convert string '"+value+"' to signed long long.", errno, strerror(errno));
	return result;
}

float convert::str2f (std::string value)
{
	if (value.empty()) throw e_convert("Can not convert empty string to float.");
	std::string::value_type * endptr;
	float result = ::strtof(value.c_str(), &endptr);
	if (*endptr) throw e_convert(std::string()+"Can not convert string '"+value+"' to float.", errno, strerror(errno));
	return result;
}

double convert::str2d (std::string value)
{
	if (value.empty()) throw e_convert("Can not convert empty string to double.");
	std::string::value_type * endptr;
	double result = ::strtod(value.c_str(), &endptr);
	if (*endptr) throw e_convert(std::string()+"Can not convert string '"+value+"' to double.", errno, strerror(errno));
	return result;
}

long double convert::str2ld (std::string value)
{
	if (value.empty()) throw e_convert("Can not convert empty string to long double.");
	std::string::value_type * endptr;
	long double result = ::strtold(value.c_str(), &endptr);
	if (*endptr) throw e_convert(std::string()+"Can not convert string '"+value+"' to long double.", errno, strerror(errno));
	return result;
}



std::string convert::ui2str (unsigned int value, int pad)
{
	try { return _sprintf("%*u", pad, value); }
	catch (e_convert &e) { throw e_convert("Can not convert unsigned int to string.", e.what()); }
}

std::string convert::si2str (signed int value, int pad)
{
	try { return _sprintf("%*d", pad, value); }
	catch (e_convert &e) { throw e_convert("Can not convert signed int to string.", e.what()); }
}

std::string convert::ul2str (unsigned long value, int pad)
{
	try { return _sprintf("%*lu", pad, value); }
	catch (e_convert &e) { throw e_convert("Can not convert unsigned long to string.", e.what()); }
}

std::string convert::sl2str (signed long value, int pad)
{
	try { return _sprintf("%*ld", pad, value); }
	catch (e_convert &e) { throw e_convert("Can not convert signed long to string.", e.what()); }
}

std::string convert::ull2str (unsigned long long value, int pad)
{
	try { return _sprintf("%*llu", pad, value); }
	catch (e_convert &e) { throw e_convert("Can not convert unsigned long long to string.", e.what()); }
}

std::string convert::sll2str (signed long long value, int pad)
{
	try { return _sprintf("%*lld", pad, value); }
	catch (e_convert &e) { throw e_convert("Can not convert signed long long to string.", e.what()); }
}

std::string convert::f2str (float value, int pad, int precision)
{
	try { return _sprintf("%*.*e", pad, precision, (double) value); }
	catch (e_convert &e) { throw e_convert("Can not convert float to string.", e.what()); }
}

std::string convert::d2str (double value, int pad, int precision)
{
	try { return _sprintf("%*.*e", pad, precision, value); }
	catch (e_convert &e) { throw e_convert("Can not convert double to string.", e.what()); }
}

std::string convert::ld2str (long double value, int pad, int precision)
{
	try { return _sprintf("%*.*Le", pad, precision, value); }
	catch (e_convert &e) { throw e_convert("Can not convert long double to string.", e.what()); }
}



t_ipaddr convert::str2ipaddr (std::string value)
{
	t_ipaddr result;
	try { result = (t_ipaddr) str2ul(value); }
	catch (e_convert &e)
	{
		in_addr_t addr = inet_addr(value.c_str());
		if (addr == INADDR_NONE)
			throw e_convert("Can not convert string '"+value+"' to ip address.", errno, strerror(errno));
		result = 0;
		result |= addr & 0xFF; addr >>= 8; result <<= 8;
		result |= addr & 0xFF; addr >>= 8; result <<= 8;
		result |= addr & 0xFF; addr >>= 8; result <<= 8;
		result |= addr & 0xFF;
	}
	return result;
}

t_ipaddr convert::str2ipaddr (std::string value, t_ipaddr def)
{
	try { return str2ipaddr(value); }
	catch (e_convert) { return def; }
}

t_ipmask convert::str2ipmask (std::string value)
{
	t_ipmask result;
	try { result = (t_ipmask) str2ui(value); }
	catch (e_convert &e)
	{
		throw e_convert("Can not convert string '"+value+"' to ip mask.", e.what());
	}
	return result;
}

t_ipmask convert::str2ipmask (std::string value, t_ipmask def)
{
	try { return str2ipmask(value); }
	catch (e_convert) { return def; }
}

t_ipport convert::str2ipport (std::string value)
{
	t_ipport result;
	try { result = (t_ipport) str2ui(value); }
	catch (e_convert &e)
	{
		throw e_convert("Can not convert string '"+value+"' to ip port.", e.what());
	}
	return result;
}

t_ipport convert::str2ipport (std::string value, t_ipport def)
{
	try { return str2ipport(value); }
	catch (e_convert) { return def; }
}

t_proto convert::str2proto (std::string value)
{
	t_proto result;
	try { result = (t_proto) str2ui(value); }
	catch (e_convert &e)
	{
		if (value == "smb" ) return proto_smb ; else
		if (value == "ftp" ) return proto_ftp ; else
		if (value == "http") return proto_http; else
		throw e_convert("Can not convert string '"+value+"' to protocol.", e.what());
	}
	return result;
}

t_proto convert::str2proto (std::string value, t_proto def)
{
	try { return str2proto(value); }
	catch (e_convert) { return def; }
}

t_path convert::str2path (std::string value)
{
	t_path result = (t_path) _split(value, "/");
	return result;
}

t_path convert::str2path (std::string value, t_path def)
{
	try { return str2path(value); }
	catch (e_convert) { return def; }
}

t_time convert::str2time (std::string value)
{
	t_time result;
	try { result = (t_time) str2si(value); }
	catch (e_convert &e)
	{
		throw e_convert("Can not convert string '"+value+"' to timestamp.", e.what());
	}
	return result;
}

t_time convert::str2time (std::string value, t_time def)
{
	try { return str2time(value); }
	catch (e_convert) { return def; }
}

t_size convert::str2size (std::string value)
{
	t_size result;
	try { result = (t_size) str2ui(value); }
	catch (e_convert &e)
	{
		throw e_convert("Can not convert string '"+value+"' to size.", e.what());
	}
	return result;
}

t_size convert::str2size (std::string value, t_size def)
{
	try { return str2size(value); }
	catch (e_convert) { return def; }
}

t_sqlid convert::str2sqlid (std::string value)
{
	t_sqlid result;
	try { result = (t_sqlid) str2ul(value); }
	catch (e_convert &e)
	{
		throw e_convert("Can not convert string '"+value+"' to sql identifier.", e.what());
	}
	return result;
}

t_sqlid convert::str2sqlid (std::string value, t_sqlid def)
{
	try { return str2sqlid(value); }
	catch (e_convert) { return def; }
}

t_pident convert::str2pident (std::string value)
{
	t_pident result;
	try { result = (t_pident) str2si(value); }
	catch (e_convert &e)
	{
		throw e_convert("Can not convert string '"+value+"' to process identifier.", e.what());
	}
	return result;
}

t_pident convert::str2pident (std::string value, t_pident def)
{
	try { return str2pident(value); }
	catch (e_convert) { return def; }
}

t_pstatus convert::str2pstatus (std::string value)
{
	t_pstatus result;
	try { result = (t_pstatus) str2si(value); }
	catch (e_convert &e)
	{
		throw e_convert("Can not convert string '"+value+"' to process status.", e.what());
	}
	return result;
}

t_pstatus convert::str2pstatus (std::string value, t_pstatus def)
{
	try { return str2pstatus(value); }
	catch (e_convert) { return def; }
}

t_fd convert::str2fd (std::string value)
{
	t_fd result;
	try { result = (t_fd) str2si(value); }
	catch (e_convert &e)
	{
		throw e_convert("Can not convert string '"+value+"' to file descriptor.", e.what());
	}
	return result;
}

t_fd convert::str2fd (std::string value, t_fd def)
{
	try { return str2fd(value); }
	catch (e_convert) { return def; }
}

t_flag convert::str2flag (std::string value)
{
	return !value.empty() && (value == "1" || value == "+"
		|| value == "t" || value == "T" || value == "true" || value == "TRUE" || value == "True"
		|| value == "y" || value == "Y" || value == "yes"  || value == "YES"  || value == "Yes"
		);
}

t_flag convert::str2flag (std::string value, t_flag def)
{
	try { return str2flag(value); }
	catch (e_convert) { return def; }
}

t_timeout convert::str2timeout (std::string value)
{
	t_timeout result;
	try { result = (t_timeout) str2ul(value); }
	catch (e_convert &e)
	{
		throw e_convert("Can not convert string '"+value+"' to timeout.", e.what());
	}
	return result;
}

t_timeout convert::str2timeout (std::string value, t_timeout def)
{
	try { return str2timeout(value); }
	catch (e_convert) { return def; }
}

t_depth convert::str2depth (std::string value)
{
	t_depth result;
	try { result = (t_depth) str2ul(value); }
	catch (e_convert &e)
	{
		throw e_convert("Can not convert string '"+value+"' to depth.", e.what());
	}
	return result;
}

t_depth convert::str2depth (std::string value, t_depth def)
{
	try { return str2depth(value); }
	catch (e_convert) { return def; }
}



std::string convert::ipaddr2print (t_ipaddr value)
{
	std::string result;
	in_addr_t addr = 0;
	addr |= value & 0xFF; value >>= 8; addr <<= 8;
	addr |= value & 0xFF; value >>= 8; addr <<= 8;
	addr |= value & 0xFF; value >>= 8; addr <<= 8;
	addr |= value & 0xFF;
	struct in_addr addrstruct; addrstruct.s_addr = addr;
	result = inet_ntoa(addrstruct);
	return result;
}

std::string convert::ipaddr2system (t_ipaddr value)
{
	try { return ul2str(value); }
	catch (e_convert &e) { throw e_convert("Can not convert ip address to system string.", e.what()); }
}

std::string convert::ipmask2print (t_ipmask value)
{
	/* convert to dot notation here??? */
	try { return ui2str(value); }
	catch (e_convert &e) { throw e_convert("Can not convert ip mask to printable string.", e.what()); }
}

std::string convert::ipmask2system (t_ipmask value)
{
	try { return ui2str(value); }
	catch (e_convert &e) { throw e_convert("Can not convert ip mask to system string.", e.what()); }
}

std::string convert::ipport2print (t_ipport value)
{
	/* convert to service name here??? */
	try { return ui2str(value); }
	catch (e_convert &e) { throw e_convert("Can not convert ip port to printable string.", e.what()); }
}

std::string convert::ipport2system (t_ipport value)
{
	try { return ui2str(value); }
	catch (e_convert &e) { throw e_convert("Can not convert ip port to system string.", e.what()); }
}

std::string convert::proto2print (t_proto value)
{
	switch (value)
	{
		case proto_smb : return "smb" ;
		case proto_ftp : return "ftp" ;
		case proto_http: return "http";
		default: return "unknown";
	}
}

std::string convert::proto2system (t_proto value)
{
	try { return ui2str(value); }
	catch (e_convert &e) { throw e_convert("Can not convert protocol to system string.", e.what()); }
}

std::string convert::path2print (t_path value)
{
	std::string result;
	for (t_path::const_iterator i = value.begin(); i != value.end(); i++)
		result = result + "/" + *i;
	if (result.empty()) result = "/";
	return result;
}

std::string convert::path2system (t_path value)
{
	std::string result;
	for (t_path::const_iterator i = value.begin(); i != value.end(); i++)
		result = result + (result.empty() ? "" : "/") + *i;
	return result;
}

std::string convert::time2print (t_time value, std::string format)
{
	try { return _strftime(format.c_str(), value); }
	catch (e_convert &e) { throw e_convert("Can not convert timestamp to printable string.", e.what()); }
}

std::string convert::time2system (t_time value)
{
	try { return si2str(value); }
	catch (e_convert &e) { throw e_convert("Can not convert timestamp to system string.", e.what()); }
}

std::string convert::size2print (t_size value, int pad)
{
	try { return ui2str(value, pad); }
	catch (e_convert &e) { throw e_convert("Can not convert size to printable string.", e.what()); }
}

std::string convert::size2system (t_size value)
{
	try { return ui2str(value); }
	catch (e_convert &e) { throw e_convert("Can not convert size to system string.", e.what()); }
}

std::string convert::sqlid2print (t_sqlid value, int pad)
{
	try { return ul2str(value, pad); }
	catch (e_convert &e) { throw e_convert("Can not convert sql identifier to printable string.", e.what()); }
}

std::string convert::sqlid2system (t_sqlid value)
{
	try { return ul2str(value); }
	catch (e_convert &e) { throw e_convert("Can not convert sql identifier to system string.", e.what()); }
}

std::string convert::pident2print (t_pident value, int pad)
{
	try { return si2str(value, pad); }
	catch (e_convert &e) { throw e_convert("Can not convert process identifier to printable string.", e.what()); }
}

std::string convert::pident2system (t_pident value)
{
	try { return si2str(value); }
	catch (e_convert &e) { throw e_convert("Can not convert process identifier to system string.", e.what()); }
}

std::string convert::pstatus2print (t_pstatus value)
{
	try { return
		(WIFEXITED  (value) && !WEXITSTATUS(value)) ? (std::string()+"success"                                   ) :
		(WIFEXITED  (value)                       ) ? (std::string()+"code "         + si2str(WEXITSTATUS(value))) :
		(WIFSIGNALED(value) && !WCOREDUMP(value)  ) ? (std::string()+"signal "       + si2str(WTERMSIG(value)   )) :
		(WIFSIGNALED(value)                       ) ? (std::string()+"signal(core) " + si2str(WTERMSIG(value)   )) :
		(WIFSTOPPED (value)                       ) ? (std::string()+"signal(stop) " + si2str(WSTOPSIG(value)   )) :
							      (std::string()+"wstatus "      + si2str(value             )) ;
	}
	catch (e_convert &e) { throw e_convert("Can not convert process status to printable string.", e.what()); }
}

std::string convert::pstatus2system (t_pstatus value)
{
	try { return si2str(value); }
	catch (e_convert &e) { throw e_convert("Can not convert process status to system string.", e.what()); }
}

std::string convert::fd2print (t_fd value, int pad)
{
	try { return si2str(value, pad); }
	catch (e_convert &e) { throw e_convert("Can not convert file descriptor to printable string.", e.what()); }
}

std::string convert::fd2system (t_fd value)
{
	try { return si2str(value); }
	catch (e_convert &e) { throw e_convert("Can not convert file descriptor to system string.", e.what()); }
}

std::string convert::flag2print (t_flag value, std::string t, std::string f)
{
	return value ? t : f;
}

std::string convert::flag2system (t_flag value)
{
	return value ? "1" : "0";
}

std::string convert::timeout2print (t_timeout value, int pad)
{
	try { return ul2str(value, pad); }
	catch (e_convert &e) { throw e_convert("Can not convert timeout to printable string.", e.what()); }
}

std::string convert::timeout2system (t_timeout value)
{
	try { return ul2str(value); }
	catch (e_convert &e) { throw e_convert("Can not convert timeout to system string.", e.what()); }
}

std::string convert::depth2print (t_depth value, int pad)
{
	try { return ul2str(value, pad); }
	catch (e_convert &e) { throw e_convert("Can not convert depth to printable string.", e.what()); }
}

std::string convert::depth2system (t_depth value)
{
	try { return ul2str(value); }
	catch (e_convert &e) { throw e_convert("Can not convert depth to system string.", e.what()); }
}
