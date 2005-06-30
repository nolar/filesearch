#ifndef _FILESEARCH_EXCEPTIONS_STREAM_TIMEOUT_
#define _FILESEARCH_EXCEPTIONS_STREAM_TIMEOUT_

#include "config.h"
#include "e_stream.h"

class e_stream_timeout : public e_stream
{
private:
protected:
public:
	e_stream_timeout (const char * a_file, unsigned a_line, std::string a_comment) : e_stream (a_file, a_line, a_comment) {}
	e_stream_timeout (const char * a_file, unsigned a_line, std::string a_comment, int a_errno) : e_stream (a_file, a_line, a_comment, a_errno) {}
	e_stream_timeout (const char * a_file, unsigned a_line, std::string a_comment, std::string a_error) : e_stream (a_file, a_line, a_comment, a_error) {}
};

#endif
