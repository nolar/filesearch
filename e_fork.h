#ifndef _FILESEARCH_EXCEPTIONS_FORK_
#define _FILESEARCH_EXCEPTIONS_FORK_

#include <string>
#include "e_basic.h"

using namespace std;

class e_fork : public e_basic
{
private:
protected:
public:
	e_fork (string a_message) : e_basic (a_message) {}
	e_fork (string a_message, int a_number, string a_error) : e_basic (a_message,a_number,a_error) {}
};

#endif
