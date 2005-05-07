#ifndef _FILESEARCH_EXCEPTIONS_FORK_
#define _FILESEARCH_EXCEPTIONS_FORK_

#include <string>
#include "config.h"
#include "e_basic.h"

class e_fork : public e_basic
{
private:
protected:
public:
	e_fork (std::string a_message) : e_basic (a_message) {}
	e_fork (std::string a_message, std::string a_error) : e_basic (a_message,a_error) {}
	e_fork (std::string a_message, int a_number, std::string a_error) : e_basic (a_message,a_number,a_error) {}
};

#endif
