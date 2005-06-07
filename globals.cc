#include "globals.h"
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <memory>

const c_identifier ipc_task_ipaddr    =  1;
const c_identifier ipc_task_ipport    =  2;
const c_identifier ipc_task_rootpath  =  3;
const c_identifier ipc_task_username  =  4;
const c_identifier ipc_task_password  =  5;
const c_identifier ipc_task_timeout   =  6;
const c_identifier ipc_task_depth     =  7;
const c_identifier ipc_task_selfname  =  8;
const c_identifier ipc_task_workgroup =  9;
const c_identifier ipc_task_bind      = 10;

const c_identifier ipc_data_event	=  1;
const c_identifier ipc_data_path	=  2;
const c_identifier ipc_data_size	=  3;
const c_identifier ipc_data_ctime	=  4;
const c_identifier ipc_data_mtime	=  5;

const int fd_ipc_task                   = 0;
const int fd_ipc_data                   = 3;
const int fd_log                        = 1;
const int fd_debug                      = 2;



long     default_timeout_task__sec	= 1;
long     default_timeout_task__usec	= 0;
unsigned default_children_count   	= 30;
unsigned default_database_cachesize_resource = 5;
unsigned default_database_cachesize_file     = 5;
char *   default_path_delimiter         = "/";
char *   default_scanner_smb            = "./filesearch-smb";
char *   default_request_username_smb   = "guest";
unsigned default_request_depth          = 100;

long   logging_timeout__sec	= 1;
long   logging_timeout__usec	= 0;
char * logging_terminator	= "\n";
char * logging_time_format	= "%Y-%m-%d %H:%M:%S";
int    logging_pid_length	= 5;



unsigned string_time_buffer_size = 1024;
unsigned string_format_buffer_size = 1024;

std::string string_time (const char * format, time_t value)
{
	std::auto_ptr<std::string::value_type> buffer(new std::string::value_type[string_time_buffer_size]);
	if (strftime(buffer.get(), string_time_buffer_size, format, localtime(&value)) < 0)
	{
		throw e_basic(__FILE__,__LINE__,std::string()+"Can not format timestamp to '"+format+"'.", errno);
	} else {
		return std::string(buffer.get());
	}
}

std::string string_format (const char * format, ...)
{
	va_list ap; va_start(ap, format);
	std::auto_ptr<std::string::value_type> buffer(new std::string::value_type[string_format_buffer_size]);
	if (vsnprintf(buffer.get(), string_format_buffer_size, format, ap) < 0)
	{
		va_end(ap);
		throw e_basic(__FILE__,__LINE__,std::string()+"Can not format string to '"+format+"'.", errno);
	} else {
		va_end(ap);
		return std::string(buffer.get());
	}
}

std::vector<std::string> string_split (std::string value, std::string delimiter)
{
	std::vector<std::string> result;
	std::string::size_type delimlen = delimiter.length(), idx, pos, prev = (std::string::size_type) -delimlen;
	do {
		idx = (prev+delimlen >= value.length()) ? std::string::npos : value.find(delimiter, prev+delimlen);
		pos = (idx == std::string::npos) ? value.length() : idx;
		std::string item = value.substr(prev+delimlen, pos-(prev+delimlen));
		if (!item.empty()) result.push_back(item);
		prev = pos;
	} while (idx != std::string::npos);
	return result;
}
