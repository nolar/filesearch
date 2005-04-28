#ifndef _FILESEARCH_OPTIONS_
#define _FILESEARCH_OPTIONS_

#include <string>
#include <vector>

using namespace std;

class options
{
private:
protected:
public:
	static unsigned default_timeout;
	static unsigned default_depth;

	static string command_smb;

	static string::value_type terminator;
	static long timeout_params_sec ;
	static long timeout_params_usec;

	static string action_code_for_resource;
	static string action_code_for_file;
	static string action_code_for_dir;
	static string action_code_for_enter;
	static string action_code_for_leave;

	static int fd_task;
	static int fd_data;
	static int fd_log;
	static int fd_debug;
	static int fd_null;

	static char * log_time_format;
	static int    log_pid_length;
};

#endif
