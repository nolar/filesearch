#ifndef _FILESEARCH_EXCEPTIONS_BASIC_
#define _FILESEARCH_EXCEPTIONS_BASIC_

#include "config.h"
#include <string>

class e_basic
{
private:
	const char * f_file;
	unsigned f_line;
	std::string f_comment;
	bool use_errno;
	bool use_error;
	int f_errno;
	std::string f_error;
protected:
public:
	e_basic (const char * a_file, unsigned a_line, std::string a_comment);
	e_basic (const char * a_file, unsigned a_line, std::string a_comment, int a_errno);
	e_basic (const char * a_file, unsigned a_line, std::string a_comment, std::string a_error);
	virtual ~e_basic () throw ();

	const char * file () const throw ();
	unsigned line () const throw ();
	virtual std::string what () const throw ();
};

#endif
