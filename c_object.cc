#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdarg.h>
#include "c_object.h"

const int c_object::_buffer_size = 1024;
const char * c_object::_time_format = "%Y-%m-%d %H:%M:%S";





c_object::c_object ()
{
}

c_object::~c_object ()
{
}





std::string c_object::_sprintf (const char * format, ...)
{
	va_list ap; va_start(ap, format);
	std::string::value_type * buffer = new std::string::value_type [_buffer_size];
	int code = ::vsnprintf(buffer, _buffer_size, format, ap);
	std::string result((code < 0) ? "" : buffer);
	delete[] buffer;
	va_end(ap);
	if (code < 0) throw e_basic(std::string()+"Can not format string to '"+format+"'.", errno, strerror(errno));
	return result;
}

std::string c_object::_strftime (const char * format, time_t value)
{
	std::string::value_type * buffer = new std::string::value_type [_buffer_size];
	int code = ::strftime(buffer, _buffer_size, (format && *format)?format:_time_format, localtime(&value));
	std::string result((code < 0) ? "" : buffer);
	delete[] buffer;
	if (code < 0) throw e_basic(std::string()+"Can not format timestamp to '"+_time_format+"'.", errno, strerror(errno));
	return result;
	
}

std::vector<std::string> c_object::_split (std::string value, std::string delimiter)
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
