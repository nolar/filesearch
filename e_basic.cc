#include "e_basic.h"





e_basic::e_basic (const char * a_file, unsigned a_line, std::string a_comment)
	: f_file(a_file)
	, f_line(a_line)
	, f_comment(a_comment)
	, use_errno(false)
	, use_error(false)
{
}

e_basic::e_basic (const char * a_file, unsigned a_line, std::string a_comment, int a_errno)
	: f_file(a_file)
	, f_line(a_line)
	, f_comment(a_comment)
	, use_errno(true)
	, use_error(false)
	, f_errno(a_errno)
{
}

e_basic::e_basic (const char * a_file, unsigned a_line, std::string a_comment, std::string a_error)
	: f_file(a_file)
	, f_line(a_line)
	, f_comment(a_comment)
	, use_errno(false)
	, use_error(true)
	, f_error(a_error)
{
}

e_basic::~e_basic () throw ()
{
}





const char * e_basic::file () const throw ()
{
	return f_file;
}

unsigned e_basic::line () const throw ()
{
	return f_line;
}

std::string e_basic::what () const throw ()
{
	std::string result = f_comment;
	if (use_errno) result = result + " " + strerror(f_errno);
	if (use_error) result = result + " " + f_error;
	return result;
}
