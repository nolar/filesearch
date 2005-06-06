#ifndef _FILESEARCH_EXCEPTIONS_DATABASE_
#define _FILESEARCH_EXCEPTIONS_DATABASE_

#include "config.h"
#include "e_basic.h"

class e_database : public e_basic
{
private:
protected:
public:
	e_database (const char * a_file, unsigned a_line, std::string a_comment) : e_basic (a_file, a_line, a_comment) {}
	e_database (const char * a_file, unsigned a_line, std::string a_comment, int a_errno) : e_basic (a_file, a_line, a_comment, a_errno) {}
	e_database (const char * a_file, unsigned a_line, std::string a_comment, std::string a_error) : e_basic (a_file, a_line, a_comment, a_error) {}
};

#endif
