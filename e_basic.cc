#include "e_basic.h"

e_basic::e_basic (string a_message)
{
	f_message = a_message;
	f_number = 0;
	f_error = "";
}

e_basic::e_basic (string a_message, string a_error)
{
	f_message = a_message;
	f_number = 0;
	f_error = a_error;
}

e_basic::e_basic (string a_message, int a_number)
{
	f_message = a_message;
	f_number = a_number;
}

e_basic::e_basic (string a_message, int a_number, string a_error)
{
	f_message = a_message;
	f_number = a_number;
	f_error = a_error;
}

e_basic::~e_basic () throw ()
{
}

string e_basic::what () throw ()
{
	if (f_error.empty()) return f_message;
	else return f_message + " " + f_error;
}

string e_basic::message ()
{
	return f_message;
}

int e_basic::number ()
{
	return f_number;
}

string e_basic::error ()
{
	return f_error;
}
