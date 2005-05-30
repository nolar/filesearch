#include "globals.h"

const c_mapkey ipc_task_ipaddr    =  1;
const c_mapkey ipc_task_ipport    =  2;
const c_mapkey ipc_task_rootpath  =  3;
const c_mapkey ipc_task_username  =  4;
const c_mapkey ipc_task_password  =  5;
const c_mapkey ipc_task_timeout   =  6;
const c_mapkey ipc_task_depth     =  7;
const c_mapkey ipc_task_selfname  =  8;
const c_mapkey ipc_task_workgroup =  9;
const c_mapkey ipc_task_bind      = 10;

const c_mapkey ipc_data_action	=  1;
const c_mapkey ipc_data_path	=  2;
const c_mapkey ipc_data_size	=  3;
const c_mapkey ipc_data_ctime	=  4;
const c_mapkey ipc_data_mtime	=  5;

const c_action action_enter		= 1;
const c_action action_leave		= 2;
const c_action action_resource		= 3;
const c_action action_dir		= 4;
const c_action action_file		= 5;

const c_protocol protocol_smb  = 1;
const c_protocol protocol_ftp  = 2;
const c_protocol protocol_http = 3;



long     default_timeout_task__sec	= 1;
long     default_timeout_task__usec	= 0;
unsigned default_children_count   	= 10;
char *   default_username_smb		= "guest";

long   logging_timeout__sec	= 1;
long   logging_timeout__usec	= 0;
char * logging_terminator	= "\n";
char * logging_time_format	= "%Y-%m-%d %H:%M:%S";
int    logging_pid_length	= 5;

