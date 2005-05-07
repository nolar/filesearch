#ifndef _FILESEARCH_OPTIONS_
#define _FILESEARCH_OPTIONS_

#include <string>
#include <vector>
#include "config.h"
#include "typedefs.h"

using namespace std;//!!!

class options
{
private:
protected:
public:
	static unsigned default_children;
	static t_timeout default_timeout;
	static t_depth   default_depth  ;

	static string command_smb;

	static string::value_type terminator;
	static long timeout_params_sec ;
	static long timeout_params_usec;

	static string action_code_for_resource;
	static string action_code_for_file;
	static string action_code_for_dir;
	static string action_code_for_enter;
	static string action_code_for_leave;

//	static int fd_task;
//	static int fd_data;
//	static int fd_log;
//	static int fd_debug;
//	static int fd_null;

//	static char * log_time_format;
//	static int    log_pid_length;

	static std::string smb_guestusername;

	static vector<t_sqlid>::size_type database_cachesize_resource;
	static vector<t_sqlid>::size_type database_cachesize_file;
};

#endif
