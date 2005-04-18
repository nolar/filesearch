#ifndef _FILESEARCH_EXCEPTIONS_ICONV_
#define _FILESEARCH_EXCEPTIONS_ICONV_

#include <string>
#include "e_basic.h"

using namespace std;

class e_iconv : public e_basic
{
private:
protected:
public:
	e_iconv (string a_message) : e_basic (a_message) {}
	e_iconv (string a_message, string a_error) : e_basic (a_message,a_error) {}
};

#endif
