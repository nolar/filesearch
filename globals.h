#ifndef _FILESEARCH_TYPEDEFS_
#define _FILESEARCH_TYPEDEFS_

#include <string>
#include <vector>
#include <map>
#include <sys/types.h>

#include "config.h"
#include "c_stream.h"
#include "c_mapkey.h"
#include "c_signed.h"
#include "c_unsigned.h"
#include "c_string.h"
#include "c_action.h"
#include "c_protocol.h"

/*!!!typedef unsigned long t_sqlid;
typedef unsigned long t_ipaddr;
typedef unsigned int  t_ipmask;
typedef unsigned int  t_ipport;
typedef std::string   t_string;
typedef std::vector<t_string> t_strings;

typedef enum {proto_unknown, proto_smb, proto_ftp, proto_http} t_proto;
typedef std::vector<std::string> t_path;
*/

extern const c_mapkey ipc_task_ipaddr   ; /* smb,ftp */
extern const c_mapkey ipc_task_ipport   ; /*     ftp */
extern const c_mapkey ipc_task_rootpath ; /* smb,ftp */
extern const c_mapkey ipc_task_username ; /* smb,ftp */
extern const c_mapkey ipc_task_password ; /* smb,ftp */
extern const c_mapkey ipc_task_timeout  ; /* smb,ftp */
extern const c_mapkey ipc_task_depth    ; /* smb,ftp */
extern const c_mapkey ipc_task_selfname ; /* smb     */
extern const c_mapkey ipc_task_workgroup; /* smb     */
extern const c_mapkey ipc_task_bind     ; /*     ftp */

extern const c_mapkey ipc_data_action   ;
extern const c_mapkey ipc_data_path     ;
extern const c_mapkey ipc_data_size     ;
extern const c_mapkey ipc_data_ctime    ;
extern const c_mapkey ipc_data_mtime    ;



extern const c_action action_enter;
extern const c_action action_leave;
extern const c_action action_resource;
extern const c_action action_dir;
extern const c_action action_file;

extern const c_protocol protocol_smb;
extern const c_protocol protocol_ftp;
extern const c_protocol protocol_http;



extern long     default_timeout_task__sec;
extern long     default_timeout_task__usec;
extern unsigned default_children_count;
extern char *   default_username_smb;

extern long   logging_timeout__sec;
extern long   logging_timeout__usec;
extern char * logging_terminator;
extern char * logging_time_format;
extern int    logging_pid_length;


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
