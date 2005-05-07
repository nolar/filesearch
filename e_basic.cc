#include "e_basic.h"

e_basic::e_basic (std::string a_message)
{
	f_message = a_message;
	f_number = 0;
	f_error = "";
}

e_basic::e_basic (std::string a_message, std::string a_error)
{
	f_message = a_message;
	f_number = 0;
	f_error = a_error;
}

e_basic::e_basic (std::string a_message, int a_number)
{
	f_message = a_message;
	f_number = a_number;
}

e_basic::e_basic (std::string a_message, int a_number, std::string a_error)
{
	f_message = a_message;
	f_number = a_number;
	f_error = a_error;
}

e_basic::~e_basic () throw ()
{
}



std::string e_basic::what () const throw ()
{
	if (f_error.empty()) return f_message;
	else return f_message + " " + f_error;
}



std::string e_basic::message ()
{
	return f_message;
}

std::string e_basic::error ()
{
	return f_error;
}

int e_basic::number ()
{
	return f_number;
}
