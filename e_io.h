#ifndef _FILESEARCH_EXCEPTIONS_IO_
#define _FILESEARCH_EXCEPTIONS_IO_

#include <string>
#include "config.h"
#include "e_basic.h"

class e_io : public e_basic
{
private:
protected:
public:
	e_io (std::string a_message) : e_basic (a_message) {}
	e_io (std::string a_message, std::string a_error) : e_basic (a_message,a_error) {}
	e_io (std::string a_message, int a_number, std::string a_error) : e_basic (a_message,a_number,a_error) {}
};

#endif
