#ifndef _FILESEARCH_EXCEPTIONS_BASIC_
#define _FILESEARCH_EXCEPTIONS_BASIC_

#include <string>

using namespace std;

class e_basic
{
private:
	string f_message;
	string f_error;
	int f_number;
protected:
public:
	e_basic (string a_message);
	e_basic (string a_message, string a_error);
	e_basic (string a_message, int a_number);
	e_basic (string a_message, int a_number, string a_error);
	virtual ~e_basic () throw ();

	virtual string what () throw ();
	string message ();
	int number ();
	string error ();
};

#endif
