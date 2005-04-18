#ifndef _FILESEARCH_EXCEPTIONS_DATABASE_
#define _FILESEARCH_EXCEPTIONS_DATABASE_

#include <string>
#include "e_basic.h"

using namespace std;

class e_database : public e_basic
{
private:
protected:
public:
	e_database (string a_message) : e_basic (a_message) {}
	e_database (string a_message, string a_error) : e_basic (a_message,a_error) {}
};

#endif
