#ifndef _FILESEARCH_TYPEDEFS_
#define _FILESEARCH_TYPEDEFS_

#include "config.h"
#include <string>
#include <vector>
#include <map>
#include <sys/types.h>
//#include "c_stream.h"
#include "c_identifier.h"
//#include "c_signed.h"
//#include "c_unsigned.h"
//#include "c_string.h"
//#include "c_event.h"
//#include "c_protocol.h"

/*!!!typedef unsigned long t_sqlid;
typedef unsigned long t_ipaddr;
typedef unsigned int  t_ipmask;
typedef unsigned int  t_ipport;
typedef std::string   t_string;
typedef std::vector<t_string> t_strings;

typedef enum {proto_unknown, proto_smb, proto_ftp, proto_http} t_proto;
typedef std::vector<std::string> t_path;
*/

extern const c_identifier ipc_task_ipaddr   ; /* smb,ftp */
extern const c_identifier ipc_task_ipport   ; /*     ftp */
extern const c_identifier ipc_task_rootpath ; /* smb,ftp */
extern const c_identifier ipc_task_username ; /* smb,ftp */
extern const c_identifier ipc_task_password ; /* smb,ftp */
extern const c_identifier ipc_task_timeout  ; /* smb,ftp */
extern const c_identifier ipc_task_depth    ; /* smb,ftp */
extern const c_identifier ipc_task_selfname ; /* smb     */
extern const c_identifier ipc_task_workgroup; /* smb     */
extern const c_identifier ipc_task_bind     ; /*     ftp */

extern const c_identifier ipc_data_event   ;
extern const c_identifier ipc_data_path     ;
extern const c_identifier ipc_data_size     ;
extern const c_identifier ipc_data_ctime    ;
extern const c_identifier ipc_data_mtime    ;

extern const int fd_ipc_task;
extern const int fd_ipc_data;
extern const int fd_log;
extern const int fd_debug;

extern long     default_timeout_task__sec;
extern long     default_timeout_task__usec;
extern unsigned default_children_count;
extern char *   default_path_delimiter;
extern char *   default_scanner_smb;
extern char *   default_request_username_smb;
extern unsigned default_request_depth;

extern long   logging_timeout__sec;
extern long   logging_timeout__usec;
extern char * logging_terminator;
extern char * logging_time_format;
extern int    logging_pid_length;

extern unsigned string_time_buffer_size;
extern unsigned string_format_buffer_size;
extern std::string string_time (const char * format, time_t value);
extern std::string string_format (const char * format, ...);
extern std::vector<std::string> string_split (std::string value, std::string delimiter);


/*!!!!!!!!


//typedef time_t t_time   ;
typedef size_t t_size   ;
typedef pid_t  t_pident ;
typedef int    t_pstatus;
typedef int    t_fd     ;
typedef bool   t_flag   ;
typedef unsigned long t_timeout;
typedef unsigned long t_depth;

*/

#endif
