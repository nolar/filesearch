#ifndef _FILESEARCH_EXCEPTIONS_STREAM_
#define _FILESEARCH_EXCEPTIONS_STREAM_

#include <string>
#include "e_basic.h"

using namespace std;

class e_stream : public e_basic
{
private:
protected:
public:
	e_stream (string a_message) : e_basic (a_message) {}
	e_stream (string a_message, int a_number, string a_error) : e_basic (a_message,a_number,a_error) {}
};

#endif
