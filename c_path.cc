#include <string.h>
#include "c_path.h"

c_path::c_path ()
	: c_object
	, f_value()
{
}

c_path::c_path (char * value)
	: c_object()
{
	f_value = _split(value, "/");
}

c_path::c_path (std::string value)
	: c_object()
{
	f_value = _split(value, "/");
}






bool c_path::stream_vary () const
{
	return true;
}

t_object_size c_path::stream_size () const
{
	t_object_size result = sizeof(std::vector<std::string>::size_type);
	for (std::vector<std::string>::const_iterator i = f_value.begin(); i != f_value.end(); i++)
		result += sizeof(std::string::size_type) + i->length();
	return result;
}

void c_path::stream_getdata (void * buffer, t_object_size size) const
{
	char * buff = static_cast<char*>(buffer);

	std::vector<std::string>::size_type count = f_value.size();
	memcpy(buff, &count, sizeof(count)); buff += sizeof(count);

	for (std::vector<std::string>::const_iterator i = f_value.begin(); i != f_value.end(); i++)
	{
		std::string::size_type len = i->length();
		memcpy(buff, &len, sizeof(len)); buff += sizeof(len);
		memcpy(buff, i->c_str(), len); buff += len;
	}
}

void c_path::stream_setdata (const void * buffer, t_object_size size)
{
	const char * buff = static_cast<const char *>(buffer);

	std::vector<std::string>::size_type count;
	memcpy(&count, buff, sizeof(count)); buff += sizeof(count);

	f_value.clear();
	for (std::vector<std::string>::size_type i = 0; i < count; i++)
	{
		std::string::size_type len;
		memcpy(&len, buff, sizeof(len)); buff += sizeof(len);
		std::string v (buff, len); buff += len;
		f_value.push_back(v);
	}
}




bool c_path::operator== (const c_path & right) const
{
	return f_value == right.f_value
	    ;
}





bool c_path::empty () const
{
	return f_value.empty();
}

void c_path::add (std::string value)
{
	f_value.push_back(value);
}




std::string c_path::basename ()
{
	return f_value.size() ? f_value[f_value.size()-1] : std::string();
}

std::string c_path::dirname (bool heading_slash, bool empty_slash)
{
	std::string result;
	if (!f_value.size()) result = empty_slash ? "/" : ""; else
	for (std::vector<std::string>::const_iterator i = f_value.begin(); (i+1) != f_value.end(); i++)
		result = result + ((heading_slash || !result.empty()) ? "/" : "") + *i;
	return result;
}

std::string c_path::ascii (bool heading_slash, bool empty_slash)
{
	std::string result;
	if (!f_value.size()) result = empty_slash ? "/" : ""; else
	for (std::vector<std::string>::const_iterator i = f_value.begin(); i != f_value.end(); i++)
		result = result + ((heading_slash || !result.empty()) ? "/" : "") + *i;
	return result;
}
