#ifndef _FILESEARCH_EXCEPTIONS_SAMBA_
#define _FILESEARCH_EXCEPTIONS_SAMBA_

#include <string>
#include "e_basic.h"

using namespace std;

class e_samba : public e_basic
{
private:
protected:
public:
	e_samba (string a_message) : e_basic (a_message) {}
	e_samba (string a_message, string a_error) : e_basic (a_message,a_error) {}
};

#endif
