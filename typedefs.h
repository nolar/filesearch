#ifndef _FILESEARCH_TYPEDEFS_
#define _FILESEARCH_TYPEDEFS_

#include <string>
#include <vector>
#include <sys/types.h>

typedef unsigned long t_sqlid;
typedef unsigned long t_ipaddr;
typedef unsigned int  t_ipmask;
typedef unsigned int  t_ipport;

typedef enum {proto_unknown, proto_smb, proto_ftp, proto_http} t_proto;
typedef std::vector<std::string> t_path;

typedef time_t t_time   ;
typedef size_t t_size   ;
typedef pid_t  t_pident ;
typedef int    t_pstatus;
typedef int    t_fd     ;
typedef bool   t_flag   ;
typedef unsigned long t_timeout;
typedef unsigned long t_depth;

#endif
