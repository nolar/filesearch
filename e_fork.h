#ifndef _FILESEARCH_EXCEPTIONS_FORK_
#define _FILESEARCH_EXCEPTIONS_FORK_

#include "config.h"
#include "e_basic.h"

class e_fork : public e_basic
{
private:
protected:
public:
	e_fork (const char * a_file, unsigned a_line, std::string a_comment) : e_basic (a_file, a_line, a_comment) {}
	e_fork (const char * a_file, unsigned a_line, std::string a_comment, int a_errno) : e_basic (a_file, a_line, a_comment, a_errno) {}
	e_fork (const char * a_file, unsigned a_line, std::string a_comment, std::string a_error) : e_basic (a_file, a_line, a_comment, a_error) {}
};

#endif
