#ifndef _FILESEARCH_EXCEPTIONS_BASIC_
#define _FILESEARCH_EXCEPTIONS_BASIC_

#include <string>
#include <exception>
#include "config.h"

class e_basic
{
private:
	std::string f_message;
	std::string f_error;
	int f_number;
protected:
public:
	e_basic (std::string a_message);
	e_basic (std::string a_message, std::string a_error);
	e_basic (std::string a_message, int a_number);
	e_basic (std::string a_message, int a_number, std::string a_error);
	virtual ~e_basic () throw ();

	virtual std::string what () const throw ();

	std::string message ();
	std::string error ();
	int number ();
};

#endif
