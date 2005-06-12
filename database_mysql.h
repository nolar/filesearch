#ifndef _FILESEARCH_DATABASE_MYSQL_
#define _FILESEARCH_DATABASE_MYSQL_

#include "database.h"
#include <mysql.h>
#include <vector>
#include <string>

class c_database_mysql : public c_database {
private:
	MYSQL handle;
	MYSQL_STMT * stmt_queries    ;
	MYSQL_STMT * stmt_startup     ;
	MYSQL_STMT * stmt_status_check;
	MYSQL_STMT * stmt_status_renew;
	MYSQL_STMT * stmt_status_clean;
	MYSQL_STMT * stmt_resource_find;
	MYSQL_STMT * stmt_resource_add;
	MYSQL_STMT * stmt_resource_index;
	MYSQL_STMT * stmt_resource_loose;
	MYSQL_STMT * stmt_resource_loos1;
	MYSQL_STMT * stmt_resource_loos2;
	MYSQL_STMT * stmt_resource_touch;
	MYSQL_STMT * stmt_file_find;
	MYSQL_STMT * stmt_file_add;
	MYSQL_STMT * stmt_file_index;
	MYSQL_STMT * stmt_file_loose;
	MYSQL_STMT * stmt_file_touch;
	MYSQL_STMT * stmt_file_change;
	struct {
		struct {
			my_bool null_id       ; long   data_id       ;
			my_bool null_protocol ; long   data_protocol ;
			my_bool null_isnetwork; long   data_isnetwork;
			my_bool null_ipaddr   ; char * data_ipaddr   ; unsigned long length_ipaddr   ; std::auto_ptr<char> auto_ipaddr   ;
			my_bool null_ipmask   ; long   data_ipmask   ;
			my_bool null_ipport   ; long   data_ipport   ;
			my_bool null_share    ; char * data_share    ; unsigned long length_share    ; std::auto_ptr<char> auto_share    ;
			my_bool null_username ; char * data_username ; unsigned long length_username ; std::auto_ptr<char> auto_username ;
			my_bool null_password ; char * data_password ; unsigned long length_password ; std::auto_ptr<char> auto_password ;
			my_bool null_workgroup; char * data_workgroup; unsigned long length_workgroup; std::auto_ptr<char> auto_workgroup;
			my_bool null_selfname ; char * data_selfname ; unsigned long length_selfname ; std::auto_ptr<char> auto_selfname ;
			my_bool null_timeout  ; long   data_timeout  ;
			my_bool null_depth    ; long   data_depth    ;
		} queries;
		struct {
			my_bool null_startup; MYSQL_TIME data_startup;
		} startup;
		struct {
			my_bool null_count   ; long       data_count   ;
			my_bool null_startup ; MYSQL_TIME data_startup ;
			my_bool null_process ; long       data_process ;
			my_bool null_protocol; long       data_protocol;
			my_bool null_ipaddr  ; char *     data_ipaddr  ; unsigned long length_ipaddr  ; std::auto_ptr<char> auto_ipaddr  ;
			my_bool null_ipport  ; long       data_ipport  ;
			my_bool null_share   ; char *     data_share   ; unsigned long length_share   ; std::auto_ptr<char> auto_share   ;
			my_bool null_username; char *     data_username; unsigned long length_username; std::auto_ptr<char> auto_username;
		} status_check;
		struct {
			my_bool null_startup ; MYSQL_TIME data_startup ;
			my_bool null_process ; long       data_process ;
			my_bool null_protocol; long       data_protocol;
			my_bool null_ipaddr  ; char *     data_ipaddr  ; unsigned long length_ipaddr  ; std::auto_ptr<char> auto_ipaddr  ;
			my_bool null_ipport  ; long       data_ipport  ;
			my_bool null_share   ; char *     data_share   ; unsigned long length_share   ; std::auto_ptr<char> auto_share   ;
			my_bool null_username; char *     data_username; unsigned long length_username; std::auto_ptr<char> auto_username;
		} status_renew;
		struct {
			my_bool null_startup ; MYSQL_TIME data_startup ;
			my_bool null_process ; long       data_process ;
		} status_clean;
		struct {
			my_bool null_protocol; long       data_protocol;
			my_bool null_ipaddr  ; char *     data_ipaddr  ; unsigned long length_ipaddr  ; std::auto_ptr<char> auto_ipaddr  ;
			my_bool null_ipport  ; long       data_ipport  ;
			my_bool null_share   ; char *     data_share   ; unsigned long length_share   ; std::auto_ptr<char> auto_share   ;
			my_bool null_username; char *     data_username; unsigned long length_username; std::auto_ptr<char> auto_username;
			my_bool null_id      ; long       data_id      ;
			my_bool null_lost    ; MYSQL_TIME data_lost    ;
		} resource_find;
		struct {
			my_bool null_query ; long       data_query ;
			my_bool null_ipaddr  ; char *     data_ipaddr  ; unsigned long length_ipaddr  ; std::auto_ptr<char> auto_ipaddr  ;
			my_bool null_share   ; char *     data_share   ; unsigned long length_share   ; std::auto_ptr<char> auto_share   ;
		} resource_add;
		struct {
			my_bool null_id      ; long       data_id      ;
			my_bool null_key     ; char *     data_key     ; unsigned long length_key     ; std::auto_ptr<char> auto_key     ;
		} resource_index;
		struct {
			my_bool null_ipaddr  ; char *     data_ipaddr  ; unsigned long length_ipaddr  ; std::auto_ptr<char> auto_ipaddr  ;
			my_bool null_seen    ; MYSQL_TIME data_seen    ;
		} resource_loose;
		struct {
			my_bool null_ipaddr  ; char *     data_ipaddr  ; unsigned long length_ipaddr  ; std::auto_ptr<char> auto_ipaddr  ;
			my_bool null_id      ; long       data_id      ;
		} resource_loos1;
		struct {
			my_bool null_resource; long       data_resource;
		} resource_loos2;
		struct {
			my_bool null_id      ; long       data_id      ;
		} resource_touch;
		struct {
			my_bool null_resource; long       data_resource;
			my_bool null_path    ; char *     data_path    ; unsigned long length_path    ; std::auto_ptr<char> auto_path    ;
			my_bool null_id      ; long       data_id      ;
			my_bool null_lost    ; MYSQL_TIME data_lost    ;
			my_bool null_container; long       data_container;
			my_bool null_size     ; long long  data_size   ;
			my_bool null_ctime   ; MYSQL_TIME data_ctime   ;
			my_bool null_mtime   ; MYSQL_TIME data_mtime   ;
		} file_find;
		struct {
			my_bool null_resource; long       data_resource;
			my_bool null_path    ; char *     data_path    ; unsigned long length_path    ; std::auto_ptr<char> auto_path    ;
			my_bool null_name    ; char *     data_name    ; unsigned long length_name    ; std::auto_ptr<char> auto_name    ;
			my_bool null_container; long       data_container;
			my_bool null_size     ; long long  data_size   ;
			my_bool null_ctime   ; MYSQL_TIME data_ctime   ;
			my_bool null_mtime   ; MYSQL_TIME data_mtime   ;
		} file_add;
		struct {
			my_bool null_id      ; long       data_id      ;
			my_bool null_key     ; char *     data_key     ; unsigned long length_key     ; std::auto_ptr<char> auto_key     ;
		} file_index;
		struct {
			my_bool null_resource; long       data_resource;
			my_bool null_seen    ; MYSQL_TIME data_seen    ;
		} file_loose;
		struct {
			my_bool null_id      ; long       data_id      ;
		} file_touch;
		struct {
			my_bool null_container; long       data_container;
			my_bool null_size     ; long long  data_size   ;
			my_bool null_ctime   ; MYSQL_TIME data_ctime   ;
			my_bool null_mtime   ; MYSQL_TIME data_mtime   ;
			my_bool null_id      ; long       data_id      ;
		} file_change;
	} binds;
	MYSQL_STMT * _stmt_make (MYSQL_BIND * params, MYSQL_BIND * results, const char * stmtname, std::string query);
	void _stmt_execute (MYSQL_STMT * stmt, const char * stmtname);
	bool _stmt_fetch (MYSQL_STMT * stmt, const char * stmtname);
	//
	std::vector<c_unsigned> _resource_cache;
	bool _resource_find  (c_unsigned & id, bool & changed, c_query query, c_string share);
	bool _resource_add   (c_unsigned & id, c_query query, c_string share);
	void _resource_index (c_unsigned   id, c_string share);
	void _resource_loose (c_query query);
	void _resource_loosf (c_query query);
	void _resource_touch (std::vector<c_unsigned> ids);
	void _resource_flush (bool forced);

	std::vector<c_unsigned> _file_cache;
	bool _file_find   (c_unsigned & id, bool & changed, c_query query, c_fileinfo fileinfo);
	bool _file_add    (c_unsigned & id, c_query query, c_fileinfo fileinfo);
	void _file_index  (c_unsigned   id, c_fileinfo fileinfo);
	void _file_loose  (c_query query);
	void _file_touch  (std::vector<c_unsigned> ids);
	void _file_flush  (bool forced);
	void _file_change (c_unsigned id, c_fileinfo fileinfo);
protected:
	//
	c_string f_host;
	c_string f_user;
	c_string f_pass;
	c_string f_db;
	c_unsigned f_port;
	c_string f_socket;
	c_unsigned f_flags;
	// field for per-thread initialization
	static pid_t _initialized_pid;
	// supplimentary routines
	c_string escape (c_string value);
	c_string quote  (c_string value);
//	string stamp  (t_time value);
///	void thread_lock ();
///	void thread_unlock ();
public:
	c_database_mysql (c_string host, c_string user, c_string pass, c_string db, c_unsigned port, c_string socket, c_unsigned flags);
	virtual ~c_database_mysql ();
	virtual c_database * duplicate ();

	// Functions to fethc basic data
	virtual c_stamp    fetch_startup  ();
	virtual c_queries fetch_queries ();

	// Functions to work with temporary per-process host/share status.
	virtual bool status_check (c_query query);
	virtual void status_renew (c_query query);
	virtual void status_clean ();
	//
	virtual c_unsigned report_share (c_query query, c_string share);
	virtual c_unsigned report_file  (c_query query, c_fileinfo fileinfo);
	virtual void       flush_shares (c_query query);
	virtual void       flush_files  (c_query query);
};

#endif
