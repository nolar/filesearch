#ifndef _FILESEARCH_TYPEDEFS_
#define _FILESEARCH_TYPEDEFS_

#include <string>
#include <vector>
#include <map>
#include <sys/types.h>
#include "e_basic.h"

typedef unsigned long t_sqlid;
typedef unsigned long t_ipaddr;
typedef unsigned int  t_ipmask;
typedef unsigned int  t_ipport;
typedef std::string   t_string;
typedef std::vector<t_string> t_strings;

typedef enum {proto_unknown, proto_smb, proto_ftp, proto_http} t_proto;
typedef std::vector<std::string> t_path;



typedef c_unsigned                     t_ipc_code ;
typedef c_string                       t_ipc_val  ;
typedef std::map<t_ipc_code,t_ipc_val> t_ipc_map  ;
typedef std::vector<t_ipc_val>         t_ipc_vec  ;

extern const t_ipc_code ipc_code_ipaddr   ; /* smb,ftp */
extern const t_ipc_code ipc_code_ipport   ; /*     ftp */
extern const t_ipc_code ipc_code_root     ; /* smb,ftp */
extern const t_ipc_code ipc_code_username ; /* smb,ftp */
extern const t_ipc_code ipc_code_password ; /* smb,ftp */
extern const t_ipc_code ipc_code_timeout  ; /* smb,ftp */
extern const t_ipc_code ipc_code_depth    ; /* smb,ftp */
extern const t_ipc_code ipc_code_selfname ; /* smb     */
extern const t_ipc_code ipc_code_workgroup; /* smb     */
extern const t_ipc_code ipc_code_bind     ; /*     ftp */




//typedef time_t t_time   ;
typedef size_t t_size   ;
typedef pid_t  t_pident ;
typedef int    t_pstatus;
typedef int    t_fd     ;
typedef bool   t_flag   ;
typedef unsigned long t_timeout;
typedef unsigned long t_depth;

class t_time
{
private:
	int f_year;
	int f_month;
	int f_day;
	int f_hour;
	int f_minute;
	int f_second;
protected:
public:
	t_time (bool now = false);
	t_time (time_t value);
	t_time (int year, int month, int day, int hour = 0, int minute = 0, int second = 0);
	t_time & operator= (time_t & right);
	t_time & operator= (t_time & right);
};


enum t_variant_type {vt_null, vt_int, vt_long};
struct t_variant;
struct t_variant
{
	t_variant_type type;
	union {
		int as_int;
		long as_long;
		t_variant as_vector;
	};
};


#endif
