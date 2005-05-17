#ifndef _FILESEARCH_TYPEDEFS_
#define _FILESEARCH_TYPEDEFS_

#include <string>
#include <vector>
#include <map>
#include <sys/types.h>
#include "config.h"
#include "c_mapkey.h"

/*!!!typedef unsigned long t_sqlid;
typedef unsigned long t_ipaddr;
typedef unsigned int  t_ipmask;
typedef unsigned int  t_ipport;
typedef std::string   t_string;
typedef std::vector<t_string> t_strings;

typedef enum {proto_unknown, proto_smb, proto_ftp, proto_http} t_proto;
typedef std::vector<std::string> t_path;
*/

extern const c_mapkey ipc_code_ipaddr   ; /* smb,ftp */
extern const c_mapkey ipc_code_ipport   ; /*     ftp */
extern const c_mapkey ipc_code_rootpath ; /* smb,ftp */
extern const c_mapkey ipc_code_username ; /* smb,ftp */
extern const c_mapkey ipc_code_password ; /* smb,ftp */
extern const c_mapkey ipc_code_timeout  ; /* smb,ftp */
extern const c_mapkey ipc_code_depth    ; /* smb,ftp */
extern const c_mapkey ipc_code_selfname ; /* smb     */
extern const c_mapkey ipc_code_workgroup; /* smb     */
extern const c_mapkey ipc_code_bind     ; /*     ftp */

/*


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
