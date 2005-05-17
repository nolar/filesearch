#ifndef _FILESEARCH_OPTIONS_
#define _FILESEARCH_OPTIONS_

#include <string>
#include <vector>
#include "config.h"
#include "c_object.h"
#include "c_stopsign.h"
#include "c_mapkey.h"
#include "c_signed.h"
#include "c_unsigned.h"
#include "c_double.h"
#include "c_flag.h"
#include "c_stamp.h"
#include "c_string.h"
#include "c_ipaddr.h"
#include "c_path.h"

using namespace std;//!!!

class options
{
private:
protected:
public:
	static unsigned default_children;
	static c_unsigned default_timeout;
	static c_unsigned default_depth  ;

	static std::string command_smb;

	static std::string::value_type terminator;
	static long timeout_params_sec ;
	static long timeout_params_usec;

	static c_mapkey action_code_for_resource;
	static c_mapkey action_code_for_file;
	static c_mapkey action_code_for_dir;
	static c_mapkey action_code_for_enter;
	static c_mapkey action_code_for_leave;

//	static int fd_task;
//	static int fd_data;
//	static int fd_log;
//	static int fd_debug;
//	static int fd_null;

//	static char * log_time_format;
//	static int    log_pid_length;

	static c_string smb_guestusername;

//	static std::vector<t_sqlid>::size_type database_cachesize_resource;
//	static std::vector<t_sqlid>::size_type database_cachesize_file;

	static c_mapkey ipc_code_ipaddr   ; /* smb,ftp */
	static c_mapkey ipc_code_root     ; /* smb,ftp */
	static c_mapkey ipc_code_username ; /* smb,ftp */
	static c_mapkey ipc_code_password ; /* smb,ftp */
	static c_mapkey ipc_code_timeout  ; /* smb,ftp */
	static c_mapkey ipc_code_depth    ; /* smb,ftp */
	static c_mapkey ipc_code_selfname ; /* smb     */
	static c_mapkey ipc_code_workgroup; /* smb     */
	static c_mapkey ipc_code_ipport   ; /*     ftp */
	static c_mapkey ipc_code_bind     ; /*     ftp */
};

#endif
