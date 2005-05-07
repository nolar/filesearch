#ifndef _FILESEARCH_EXCEPTIONS_SAMBA_
#define _FILESEARCH_EXCEPTIONS_SAMBA_

#include <string>
#include "config.h"
#include "e_basic.h"

class e_samba : public e_basic
{
private:
protected:
public:
	e_samba (std::string a_message) : e_basic (a_message) {}
	e_samba (std::string a_message, std::string a_error) : e_basic (a_message,a_error) {}
	e_samba (std::string a_message, int a_number, std::string a_error) : e_basic (a_message,a_number,a_error) {}
};

#endif
