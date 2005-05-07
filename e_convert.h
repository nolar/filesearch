#ifndef _FILESEARCH_EXCEPTIONS_CONVERT_
#define _FILESEARCH_EXCEPTIONS_CONVERT_

#include <string>
#include "config.h"
#include "e_basic.h"

class e_convert : public e_basic
{
private:
protected:
public:
	e_convert (std::string a_message) : e_basic (a_message) {}
	e_convert (std::string a_message, std::string a_error) : e_basic (a_message,a_error) {}
	e_convert (std::string a_message, int a_number, std::string a_error) : e_basic (a_message,a_number,a_error) {}
};

#endif
