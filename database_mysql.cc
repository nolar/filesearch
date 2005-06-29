#include "database_mysql.h"
#include "globals.h"
#include "c_stream.h"

#include "c_signed.h"
#include "c_unsigned.h"
#include "c_double.h"
#include "c_flag.h"
#include "c_stamp.h"
#include "c_string.h"
#include "c_ipaddr.h"
#include "c_path.h"
#include "c_event.h"
#include "c_protocol.h"





#define BIND_STRING(BIND,STMTNAME,FIELDNAME,MAXLENGTH) \
	binds.STMTNAME.data_##FIELDNAME = new char[MAXLENGTH]; \
	binds.STMTNAME.auto_##FIELDNAME.reset(binds.STMTNAME.data_##FIELDNAME); \
	BIND.buffer_type = MYSQL_TYPE_STRING; \
	BIND.buffer = binds.STMTNAME.data_##FIELDNAME; \
	BIND.buffer_length = MAXLENGTH; \
	BIND.is_null = &binds.STMTNAME.null_##FIELDNAME; \
	BIND.length = &binds.STMTNAME.length_##FIELDNAME;

#define BIND_LONG(BIND,STMTNAME,FIELDNAME) \
	BIND.buffer_type = MYSQL_TYPE_LONG; \
	BIND.buffer = &binds.STMTNAME.data_##FIELDNAME; \
	BIND.is_null = &binds.STMTNAME.null_##FIELDNAME;

#define BIND_LONGLONG(BIND,STMTNAME,FIELDNAME) \
	BIND.buffer_type = MYSQL_TYPE_LONGLONG; \
	BIND.buffer = &binds.STMTNAME.data_##FIELDNAME; \
	BIND.is_null = &binds.STMTNAME.null_##FIELDNAME;

#define BIND_TIME(BIND,STMTNAME,FIELDNAME) \
	BIND.buffer_type = MYSQL_TYPE_DATETIME; \
	BIND.buffer = &binds.STMTNAME.data_##FIELDNAME; \
	BIND.is_null = &binds.STMTNAME.null_##FIELDNAME;





pid_t c_database_mysql::_initialized_pid = 0;

c_database_mysql::c_database_mysql (c_string host, c_string user, c_string pass, c_string db, c_unsigned port, c_string socket, c_unsigned flags)
	: c_database ()
	, stmt_queries    (NULL)
	, stmt_startup     (NULL)
	, stmt_status_check(NULL)
	, stmt_status_renew(NULL)
	, stmt_status_clean(NULL)
	, stmt_resource_find(NULL)
	, stmt_resource_add(NULL)
	, stmt_resource_index(NULL)
	, stmt_resource_loose(NULL)
	, stmt_resource_loos1(NULL)
	, stmt_resource_loos2(NULL)
	, stmt_resource_touch(NULL)
	, stmt_file_find(NULL)
	, stmt_file_add(NULL)
	, stmt_file_index(NULL)
	, stmt_file_loose(NULL)
	, stmt_file_touch(NULL)
	, stmt_file_change(NULL)
{
	// storing connection parameters
	f_host   = host;
	f_user   = user;
	f_pass   = pass;
	f_db     = db;
	f_port   = port;
	f_socket = socket;
	f_flags  = flags;
	// initializing database engine if it is not already initialized
	if (_initialized_pid != getpid())
	{
		if (mysql_thread_init()) {
			throw e_database(__FILE__,__LINE__,"Can not initialize mysql library for thread.", mysql_error(&handle));
		}
		_initialized_pid = getpid();
	}
	// initializing database handle
	if (mysql_init(&handle) == NULL)
	{
		throw e_database(__FILE__,__LINE__,"Can not initialize mysql library.");
	}
	// reading configs
	if (mysql_options(&handle, MYSQL_READ_DEFAULT_GROUP, "filesearch"))
	{
		throw e_database(__FILE__,__LINE__,"Can not set up mysql client options.", mysql_error(&handle));
	}
	// connecting
	if (mysql_real_connect(&handle, host.empty()?NULL:host.c_str(), user.empty()?NULL:user.c_str(), pass.empty()?NULL:pass.c_str(), db.empty()?NULL:db.c_str(), port.get(), socket.empty()?NULL:socket.c_str(), flags.get()) == NULL)
	{
		throw e_database(__FILE__,__LINE__,"Can not connect to mysql database.", mysql_error(&handle));
	}
}

c_database_mysql::~c_database_mysql ()
{
	if (stmt_queries    ) mysql_stmt_close(stmt_queries    );
	if (stmt_startup     ) mysql_stmt_close(stmt_startup     );
	if (stmt_status_check) mysql_stmt_close(stmt_status_check);
	if (stmt_status_renew) mysql_stmt_close(stmt_status_renew);
	if (stmt_status_clean) mysql_stmt_close(stmt_status_clean);
	if (stmt_resource_find ) mysql_stmt_close(stmt_resource_find );
	if (stmt_resource_add  ) mysql_stmt_close(stmt_resource_add  );
	if (stmt_resource_index) mysql_stmt_close(stmt_resource_index);
	if (stmt_resource_loose) mysql_stmt_close(stmt_resource_loose);
	if (stmt_resource_loos1) mysql_stmt_close(stmt_resource_loos1);
	if (stmt_resource_loos2) mysql_stmt_close(stmt_resource_loos2);
	if (stmt_resource_touch) mysql_stmt_close(stmt_resource_touch);
	if (stmt_file_find  ) mysql_stmt_close(stmt_file_find  );
	if (stmt_file_add   ) mysql_stmt_close(stmt_file_add   );
	if (stmt_file_index ) mysql_stmt_close(stmt_file_index );
	if (stmt_file_loose ) mysql_stmt_close(stmt_file_loose );
	if (stmt_file_touch ) mysql_stmt_close(stmt_file_touch );
	if (stmt_file_change) mysql_stmt_close(stmt_file_change);
	mysql_close(&handle);
}

c_database * c_database_mysql::duplicate ()
{
	c_database_mysql * result = new c_database_mysql(f_host, f_user, f_pass, f_db, f_port, f_socket, f_flags);
	result->status_init(f_process, f_startup);
	return result;
}





MYSQL_STMT * c_database_mysql::_stmt_make (MYSQL_BIND * params, MYSQL_BIND * results, const char * stmtname, std::string query)
{
	// allocating statement for current connection
	MYSQL_STMT * stmt = mysql_stmt_init(&handle);
	if (!stmt)
	{
		throw e_database(__FILE__,__LINE__,std::string()+"Can not allocate memory for statement '"+stmtname+"'.", mysql_error(&handle));
	}
	// parsing query for statement
	if (mysql_stmt_prepare(stmt, query.c_str(), query.length()))
	{
		std::string msg = mysql_stmt_error(stmt);
		mysql_stmt_close(stmt);
		throw e_database(__FILE__,__LINE__,std::string()+"Can not prepare statement '"+stmtname+"' ("+query+").", msg);
	}
	// binding fields with statement
	if (params)
		if (mysql_stmt_bind_param(stmt, params))
		{
			std::string msg = mysql_stmt_error(stmt);
			mysql_stmt_close(stmt);
			throw e_database(__FILE__,__LINE__,std::string()+"Can not bind parameters to statement '"+stmtname+"' ("+query+").", msg);
		}
	// binding fields with statement
	if (results)
		if (mysql_stmt_bind_result(stmt, results))
		{
			std::string msg = mysql_stmt_error(stmt);
			mysql_stmt_close(stmt);
			throw e_database(__FILE__,__LINE__,std::string()+"Can not bind results to statement '"+stmtname+"' ("+query+").", msg);
		}
	// return gotten statement
	return stmt;
}

void c_database_mysql::_stmt_execute (MYSQL_STMT * stmt, const char * stmtname)
{
	if (mysql_stmt_execute(stmt))
	{
		std::string msg = mysql_stmt_error(stmt);
		throw e_database(__FILE__,__LINE__,std::string()+"Can not execute prepared statement '"+stmtname+"'.", msg);
	}
}

bool c_database_mysql::_stmt_fetch (MYSQL_STMT * stmt, const char * stmtname)
{
	if (!stmt) throw e_database(__FILE__,__LINE__,std::string()+"Can not fetch from NULL statement '"+stmtname+"'.");
	int code = mysql_stmt_fetch(stmt);
	if (code == MYSQL_NO_DATA)
	{
		mysql_stmt_free_result(stmt);
		return false;
	}
	else
	if (code != 0)
	{
		std::string msg = mysql_stmt_error(stmt);
		mysql_stmt_free_result(stmt);
		throw e_database(__FILE__,__LINE__,std::string()+"Error while fetching from statement '"+stmtname+"'.", msg);
	}
	else return true;
}




/***************************************************************************************************
                                     DATA FETCHING ROUTINES
 ***************************************************************************************************/
c_queries c_database_mysql::fetch_queries ()
{
	c_queries result;
	// if statement is not prepared yet, preparing it now
	if (!stmt_queries)
	{
		MYSQL_BIND rbind[13]; memset(rbind, 0, sizeof(rbind));
		BIND_LONG  (rbind[ 0], queries, id       );
		BIND_LONG  (rbind[ 1], queries, protocol );
		BIND_LONG  (rbind[ 2], queries, isnetwork);
		BIND_STRING(rbind[ 3], queries, ipaddr   , 1024);
		BIND_LONG  (rbind[ 4], queries, ipmask   );
		BIND_LONG  (rbind[ 5], queries, ipport   );
		BIND_STRING(rbind[ 6], queries, share    , 1024);
		BIND_STRING(rbind[ 7], queries, username , 1024);
		BIND_STRING(rbind[ 8], queries, password , 1024);
		BIND_STRING(rbind[ 9], queries, workgroup, 1024);
		BIND_STRING(rbind[10], queries, selfname , 1024);
		BIND_LONG  (rbind[11], queries, timeout  );
		BIND_LONG  (rbind[12], queries, depth    );
		stmt_queries = _stmt_make(NULL, rbind, "queries", std::string() +
			"select f_query, f_protocol, f_isnetwork, f_ipaddr, f_ipmask, f_ipport, f_share, f_username, f_password, f_workgroup, f_selfname, f_timeout, f_depth" +
			"  from t_query " +
			" where f_active = 1 " +
			" order by f_share desc, f_ipmask desc");
	}
	// getting data
	_stmt_execute(stmt_queries, "queries");
	while (_stmt_fetch(stmt_queries, "queries"))
	{
		c_unsigned o_id        ; if (!binds.queries.null_id       ) o_id        = binds.queries.data_id        ; else throw e_database(__FILE__,__LINE__,"Identifier field of query is NULL.");
		c_protocol o_protocol  ; if (!binds.queries.null_protocol ) o_protocol  = binds.queries.data_protocol  ; else throw e_database(__FILE__,__LINE__,"Field 'protocol' is NULL." );
		c_flag     o_isnetwork ; if (!binds.queries.null_isnetwork) o_isnetwork = binds.queries.data_isnetwork ; else throw e_database(__FILE__,__LINE__,"Field 'isnetwork' is NULL.");
		c_ipaddr   o_ipaddr    ; if (!binds.queries.null_ipaddr   ) o_ipaddr    = binds.queries.null_ipmask ? c_ipaddr(std::string(binds.queries.data_ipaddr, binds.queries.length_ipaddr)) : c_ipaddr(std::string(binds.queries.data_ipaddr, binds.queries.length_ipaddr), binds.queries.data_ipmask); else throw e_database(__FILE__,__LINE__,"Field 'ipaddr' is NULL.");
		c_unsigned o_ipport    ; if (!binds.queries.null_ipport   ) o_ipport    = binds.queries.data_ipport    ;
		c_string   o_share     ; if (!binds.queries.null_share    ) o_share     = std::string(binds.queries.data_share     , binds.queries.length_share    );
		c_string   o_username  ; if (!binds.queries.null_username ) o_username  = std::string(binds.queries.data_username  , binds.queries.length_username );
		c_string   o_password  ; if (!binds.queries.null_password ) o_password  = std::string(binds.queries.data_password  , binds.queries.length_password );
		c_string   o_workgroup ; if (!binds.queries.null_workgroup) o_workgroup = std::string(binds.queries.data_workgroup , binds.queries.length_workgroup);
		c_string   o_selfname  ; if (!binds.queries.null_selfname ) o_selfname  = std::string(binds.queries.data_selfname  , binds.queries.length_selfname );
		c_unsigned o_timeout   ; if (!binds.queries.null_timeout  ) o_timeout   = binds.queries.data_timeout   ;
		c_unsigned o_depth     ; if (!binds.queries.null_depth    ) o_depth     = binds.queries.data_depth     ; else o_depth = default_query_depth;
		result.push_back(c_query(o_id, o_protocol, o_isnetwork, o_ipaddr, o_ipport, o_share, o_username, o_password, o_workgroup, o_selfname, o_timeout, o_depth));
	}
	return result;
}

c_stamp c_database_mysql::fetch_startup ()
{
	c_stamp result;
	// if statement is not prepared yet, preparing it now
	if (!stmt_startup)
	{
		MYSQL_BIND rbind[1]; memset(rbind, 0, sizeof(rbind));
		BIND_TIME(rbind[0], startup, startup);
		stmt_startup = _stmt_make(NULL, rbind, "startup", std::string() +
			"select now()");
	}
	// getting data
	_stmt_execute(stmt_startup, "startup");
	while (_stmt_fetch(stmt_startup, "startup"))
	{
		if (!binds.startup.null_startup)
			result = c_stamp(
				binds.startup.data_startup.year,
				binds.startup.data_startup.month,
				binds.startup.data_startup.day,
				binds.startup.data_startup.hour,
				binds.startup.data_startup.minute,
				binds.startup.data_startup.second);
		else throw e_database(__FILE__,__LINE__,"Startup field is NULL.");
	}
	return result;
}


/***************************************************************************************************
                                STATUS OF SCAN PROCESS ROUTINES
 ***************************************************************************************************/
bool c_database_mysql::status_check (c_query query)
{
	bool result = false;
	// if statement is not prepared yet, preparing it now
	if (!stmt_status_check)
	{
		MYSQL_BIND pbind[7]; memset(pbind, 0, sizeof(pbind));
		BIND_TIME  (pbind[0], status_check, startup );
		BIND_LONG  (pbind[1], status_check, process );
		BIND_LONG  (pbind[2], status_check, protocol);
		BIND_STRING(pbind[3], status_check, ipaddr  , 1024);
		BIND_LONG  (pbind[4], status_check, ipport  );
		BIND_STRING(pbind[5], status_check, share   , 1024);
		BIND_STRING(pbind[6], status_check, username, 1024);
		MYSQL_BIND rbind[1]; memset(rbind, 0, sizeof(rbind));
		BIND_LONG(rbind[0], status_check, count);
		stmt_status_check = _stmt_make(pbind, rbind, "status_check", std::string() +
			"select count(*) from t_status" +
			" where f_startup  = ?" + 
			"   and f_process  = ?" + 
			"   and f_protocol = ?" + 
			"   and f_ipaddr   = ?" + 
			"   and f_ipport   = ?" + 
			"   and f_share    = ?" + 
			"   and f_username = ?");
	}
	// setting parameters
	binds.status_check.null_startup = false;
		binds.status_check.data_startup.year = f_startup.get_year();
		binds.status_check.data_startup.month = f_startup.get_month();
		binds.status_check.data_startup.day = f_startup.get_day();
		binds.status_check.data_startup.hour = f_startup.get_hour();
		binds.status_check.data_startup.minute = f_startup.get_minute();
		binds.status_check.data_startup.second = f_startup.get_second();
	binds.status_check.null_process = false;
		binds.status_check.data_process = f_process.get();
	binds.status_check.null_protocol = false;
		binds.status_check.data_protocol = query.protocol().get();
	binds.status_check.null_ipaddr = false;
		strncpy(binds.status_check.data_ipaddr, query.ipaddr().ascii(-1).c_str(), 1024);
		binds.status_check.length_ipaddr = query.ipaddr().ascii(-1).length();
	binds.status_check.null_ipport = false;
		binds.status_check.data_ipport = query.ipport().get();
	binds.status_check.null_share = false;
		strncpy(binds.status_check.data_share, query.share().c_str(), 1024);
		binds.status_check.length_share = query.share().length();
	binds.status_check.null_username = false;
		strncpy(binds.status_check.data_username, query.username().c_str(), 1024);
		binds.status_check.length_username = query.username().length();
	// getting data
	_stmt_execute(stmt_status_check, "status_check");
	while (_stmt_fetch(stmt_status_check, "status_check"))
	{
		if (!binds.status_check.null_count)
		{
			if (binds.status_check.data_count)
				result = true;
		}
		else throw e_database(__FILE__,__LINE__,"Count of status records is NULL.");
	}
	return result;
}

void c_database_mysql::status_renew (c_query query)
{
	// if statement is not prepared yet, preparing it now
	if (!stmt_status_renew)
	{
		MYSQL_BIND pbind[7]; memset(pbind, 0, sizeof(pbind));
		BIND_TIME  (pbind[0], status_renew, startup );
		BIND_LONG  (pbind[1], status_renew, process );
		BIND_LONG  (pbind[2], status_renew, protocol);
		BIND_STRING(pbind[3], status_renew, ipaddr  , 1024);
		BIND_LONG  (pbind[4], status_renew, ipport  );
		BIND_STRING(pbind[5], status_renew, share   , 1024);
		BIND_STRING(pbind[6], status_renew, username, 1024);
		stmt_status_renew = _stmt_make(pbind, NULL, "status_renew", std::string() +
			"insert into t_status (f_startup,f_process,f_protocol,f_ipaddr,f_ipport,f_share,f_username)" +
			" values (?,?,?,?,?,?,?)");
	}
	// setting parameters
	binds.status_renew.null_startup = false;
		binds.status_renew.data_startup.year = f_startup.get_year();
		binds.status_renew.data_startup.month = f_startup.get_month();
		binds.status_renew.data_startup.day = f_startup.get_day();
		binds.status_renew.data_startup.hour = f_startup.get_hour();
		binds.status_renew.data_startup.minute = f_startup.get_minute();
		binds.status_renew.data_startup.second = f_startup.get_second();
	binds.status_renew.null_process = false;
		binds.status_renew.data_process = f_process.get();
	binds.status_renew.null_protocol = false;
		binds.status_renew.data_protocol = query.protocol().get();
	binds.status_renew.null_ipaddr = false;
		strncpy(binds.status_renew.data_ipaddr, query.ipaddr().ascii(-1).c_str(), 1024);
		binds.status_renew.length_ipaddr = query.ipaddr().ascii(-1).length();
	binds.status_renew.null_ipport = false;
		binds.status_renew.data_ipport = query.ipport().get();
	binds.status_renew.null_share = false;
		strncpy(binds.status_renew.data_share, query.share().c_str(), 1024);
		binds.status_renew.length_share = query.share().length();
	binds.status_renew.null_username = false;
		strncpy(binds.status_renew.data_username, query.username().c_str(), 1024);
		binds.status_renew.length_username = query.username().length();
	// getting data
	_stmt_execute(stmt_status_renew, "status_renew");
}

void c_database_mysql::status_clean ()
{
	// if statement is not prepared yet, preparing it now
	if (!stmt_status_clean)
	{
		MYSQL_BIND pbind[2]; memset(pbind, 0, sizeof(pbind));
		BIND_TIME(pbind[0], status_clean, startup );
		BIND_LONG(pbind[1], status_clean, process );
		stmt_status_clean = _stmt_make(pbind, NULL, "status_clean", std::string() +
			"delete from t_status" +
			" where f_startup = ? and f_process = ?");
	}
	// setting parameters
	binds.status_clean.null_startup = false;
		binds.status_clean.data_startup.year = f_startup.get_year();
		binds.status_clean.data_startup.month = f_startup.get_month();
		binds.status_clean.data_startup.day = f_startup.get_day();
		binds.status_clean.data_startup.hour = f_startup.get_hour();
		binds.status_clean.data_startup.minute = f_startup.get_minute();
		binds.status_clean.data_startup.second = f_startup.get_second();
	binds.status_clean.null_process = false;
		binds.status_clean.data_process = f_process.get();
	// getting data
	_stmt_execute(stmt_status_clean, "status_clean");
}


/***************************************************************************************************
                           SUPPLIMENTARY ROUTES FOR FILE REPORTING/FLUSHING
 ***************************************************************************************************/
bool c_database_mysql::_resource_find (c_unsigned & id, bool & changed, c_query query, c_string share)
{
	bool result = false;
	// if statement is not prepared yet, preparing it now
	if (!stmt_resource_find)
	{
		MYSQL_BIND pbind[5]; memset(pbind, 0, sizeof(pbind));
		BIND_LONG  (pbind[0], resource_find, protocol);
		BIND_STRING(pbind[1], resource_find, ipaddr  , 1024);
		BIND_LONG  (pbind[2], resource_find, ipport  );
		BIND_STRING(pbind[3], resource_find, share   , 1024);
		BIND_STRING(pbind[4], resource_find, username, 1024);
		MYSQL_BIND rbind[2]; memset(rbind, 0, sizeof(rbind));
		BIND_LONG(rbind[0], resource_find, id);
		BIND_TIME(rbind[1], resource_find, lost);
		stmt_resource_find = _stmt_make(pbind, rbind, "resource_find", std::string() +
			"select R.f_resource,R.f_lost from t_query Q join t_resource R using (f_query)" +
			" where Q.f_protocol = ?"
			" and   R.f_ipaddr   = ?"
			" and   Q.f_ipport   = ?"
			" and   R.f_share    = ?"
			" and   Q.f_username = ?");
	}
	// setting parameters
	binds.resource_find.null_protocol = false;
		binds.resource_find.data_protocol = query.protocol().get();
	binds.resource_find.null_ipaddr = false;
		strncpy(binds.resource_find.data_ipaddr, query.ipaddr().ascii(-1).c_str(), 1024);
		binds.resource_find.length_ipaddr = query.ipaddr().ascii(-1).length();
	binds.resource_find.null_ipport = false;
		binds.resource_find.data_ipport = query.ipport().get();
	binds.resource_find.null_share = false;
		strncpy(binds.resource_find.data_share, share.c_str(), 1024);
		binds.resource_find.length_share = share.length();
	binds.resource_find.null_username = false;
		strncpy(binds.resource_find.data_username, query.username().c_str(), 1024);
		binds.resource_find.length_username = query.username().length();
	// getting data
	_stmt_execute(stmt_resource_find, "resource_find");
	while (_stmt_fetch(stmt_resource_find, "resource_find"))
	{
		if (!binds.resource_find.null_id)
		{
			id.set(binds.resource_find.data_id);
			changed = !binds.resource_find.null_lost;
			result = true;
		}
		else throw e_database(__FILE__,__LINE__,"Returned NULL id field.");
	}
	return result;
}

bool c_database_mysql::_resource_add (c_unsigned & id, c_query query, c_string share)
{
	bool result = true;
	// if statement is not prepared yet, preparing it now
	if (!stmt_resource_add)
	{
		MYSQL_BIND pbind[3]; memset(pbind, 0, sizeof(pbind));
		BIND_LONG  (pbind[0], resource_add, query );
		BIND_STRING(pbind[1], resource_add, ipaddr  , 1024);
		BIND_STRING(pbind[2], resource_add, share   , 1024);
		stmt_resource_add = _stmt_make(pbind, NULL, "resource_add", std::string() +
			"insert into t_resource (f_query, f_ipaddr, f_share, f_found, f_seen, f_lost)" +
			" values (?,?,?,now(),now(),NULL)");
	}
	// setting parameters
	binds.resource_add.null_query = false;
		binds.resource_add.data_query = query.id().get();
	binds.resource_add.null_ipaddr = false;
		strncpy(binds.resource_add.data_ipaddr, query.ipaddr().ascii(-1).c_str(), 1024);
		binds.resource_add.length_ipaddr = query.ipaddr().ascii(-1).length();
	binds.resource_add.null_share = false;
		strncpy(binds.resource_add.data_share, share.c_str(), 1024);
		binds.resource_add.length_share = share.length();
	// getting data
	try { _stmt_execute(stmt_resource_add, "resource_add"); }
	catch (e_database) { result = false; }
	if (result)
	{
		id.set(mysql_stmt_insert_id(stmt_resource_add));
	}
	return result;
}

void c_database_mysql::_resource_index (c_unsigned id, c_string share)
{
	// if statement is not prepared yet, preparing it now
	if (!stmt_resource_index)
	{
		MYSQL_BIND pbind[2]; memset(pbind, 0, sizeof(pbind));
		BIND_LONG    (pbind[0], resource_index, id  );
		BIND_STRING  (pbind[1], resource_index, key, 4096);
		stmt_resource_index = _stmt_make(pbind, NULL, "resource_index", std::string() +
			"insert into t_resource_index (f_resource, f_key)" +
			" values (?,?)");
	}
	//
	c_string::t_value key = share.ascii();
	while (!key.empty())
	{
	// setting parameters
	binds.resource_index.null_id = false;
		binds.resource_index.data_id = id.get();
	binds.resource_index.null_key = false;
		strncpy(binds.resource_index.data_key, key.c_str(), 1024);
		binds.resource_index.length_key = key.length();
	// executing statement
	_stmt_execute(stmt_resource_index, "resource_index");
	// cutting key
	key.erase(0,1);
	}
}

void c_database_mysql::_resource_loose (c_query query)
{
	// if statement is not prepared yet, preparing it now
	if (!stmt_resource_loose)
	{
		MYSQL_BIND pbind[2]; memset(pbind, 0, sizeof(pbind));
		BIND_TIME  (pbind[0], resource_loose, seen    );
		BIND_STRING(pbind[1], resource_loose, ipaddr  , 1024);
		stmt_resource_loose = _stmt_make(pbind, NULL, "resource_loose", std::string() +
			"update t_resource" +
			"   set f_lost = now()" +
			" where f_lost is null" +
			"   and f_seen < ?" +
			"   and f_ipaddr = ?");
	}
	// setting parameters
	binds.resource_loose.null_seen = false;
		binds.resource_loose.data_seen.year = f_startup.get_year();
		binds.resource_loose.data_seen.month = f_startup.get_month();
		binds.resource_loose.data_seen.day = f_startup.get_day();
		binds.resource_loose.data_seen.hour = f_startup.get_hour();
		binds.resource_loose.data_seen.minute = f_startup.get_minute();
		binds.resource_loose.data_seen.second = f_startup.get_second();
	binds.resource_loose.null_ipaddr = false;
		strncpy(binds.resource_loose.data_ipaddr, query.ipaddr().ascii(-1).c_str(), 1024);
		binds.resource_loose.length_ipaddr = query.ipaddr().ascii(-1).length();
	// getting data
	_stmt_execute(stmt_resource_loose, "resource_loose");
}

void c_database_mysql::_resource_loosf (c_query query)
{
	std::vector<c_unsigned> ids;
	// if statement is not prepared yet, preparing it now
	if (!stmt_resource_loos1)
	{
		MYSQL_BIND pbind[1]; memset(pbind, 0, sizeof(pbind));
		BIND_STRING(pbind[0], resource_loos1, ipaddr, 1024);
		MYSQL_BIND rbind[1]; memset(rbind, 0, sizeof(rbind));
		BIND_LONG(rbind[0], resource_loos1, id);
		stmt_resource_loos1 = _stmt_make(pbind, rbind, "resource_loos1", std::string() +
			"select f_resource from t_resource" + 
			" where f_lost is not null" +
			"   and f_ipaddr = ?");
	}
	// setting parameters
	binds.resource_loos1.null_ipaddr = false;
		strncpy(binds.resource_loos1.data_ipaddr, query.ipaddr().ascii(-1).c_str(), 1024);
		binds.resource_loos1.length_ipaddr = query.ipaddr().ascii(-1).length();
	// getting data
	_stmt_execute(stmt_resource_loos1, "resource_loos1");
	while (_stmt_fetch(stmt_resource_loos1, "resource_loos1"))
	{
		if (!binds.resource_loos1.null_id)
		{
			ids.push_back(c_unsigned(binds.resource_loos1.data_id));
		}
		else throw e_database(__FILE__,__LINE__,"Returned NULL id field.");
	}

	// if statement is not prepared yet, preparing it now
	if (!stmt_resource_loos2)
	{
		MYSQL_BIND pbind[1]; memset(pbind, 0, sizeof(pbind));
		BIND_LONG(pbind[0], resource_loos2, resource);
		stmt_resource_loos2 = _stmt_make(pbind, NULL, "resource_loos2", std::string() +
			"update t_file" +
			"   set f_lost = now()" +
			" where f_lost is null" +
			"   and f_resource = ?");
	}
	// cycling throu all found resources' files
	for (std::vector<c_unsigned>::const_iterator i = ids.begin(); i != ids.end(); i++)
	{
	// setting parameters
	binds.resource_loos2.null_resource = false;
		binds.resource_loos2.data_resource = i->get();
	// getting data
	_stmt_execute(stmt_resource_loos2, "resource_loos2");
	}
}

void c_database_mysql::_resource_touch (std::vector<c_unsigned> ids)
{
	if (ids.empty()) return;
	// if statement is not prepared yet, preparing it now
	if (!stmt_resource_touch)
	{
		MYSQL_BIND pbind[1]; memset(pbind, 0, sizeof(pbind));
		BIND_LONG(pbind[0], resource_touch, id);
		stmt_resource_touch = _stmt_make(pbind, NULL, "resource_touch", std::string() +
			"update t_resource" +
			"   set f_seen = now()" +
			"     , f_lost = NULL"
			" where f_resource = ?");
	}
	for (std::vector<c_unsigned>::const_iterator i = ids.begin(); i != ids.end(); i++)
	{
	// setting parameters
	binds.resource_touch.null_id = false;
		binds.resource_touch.data_id = i->get();
	// getting data
	_stmt_execute(stmt_resource_touch, "resource_touch");
	}
}

void c_database_mysql::_resource_flush (bool forced)
{
	if ((_resource_cache.size() >= default_database_cachesize_resource) || forced)
	{
		_resource_touch(_resource_cache);
		_resource_cache.clear();
	}
}


/***************************************************************************************************
                           SUPPLIMENTARY ROUTES FOR FILE REPORTING/FLUSHING
 ***************************************************************************************************/
bool c_database_mysql::_file_find (c_unsigned & id, bool & changed, c_query query, c_fileinfo fileinfo)
{
	bool result = false;
	// if statement is not prepared yet, preparing it now
	if (!stmt_file_find)
	{
		MYSQL_BIND pbind[2]; memset(pbind, 0, sizeof(pbind));
		BIND_LONG  (pbind[0], file_find, resource);
		BIND_STRING(pbind[1], file_find, path    , 4096);
		MYSQL_BIND rbind[6]; memset(rbind, 0, sizeof(rbind));
		BIND_LONG    (rbind[0], file_find, id);
		BIND_TIME    (rbind[1], file_find, lost);
		BIND_LONG    (rbind[2], file_find, container);
		BIND_LONGLONG(rbind[3], file_find, size);
		BIND_TIME    (rbind[4], file_find, ctime);
		BIND_TIME    (rbind[5], file_find, mtime);
		stmt_file_find = _stmt_make(pbind, rbind, "file_find", std::string() +
			"select F.f_file,F.f_lost,F.f_container,F.f_size,F.f_ctime,F.f_mtime from t_resource R join t_file F using (f_resource)" +
			" where F.f_resource = ?" +
			"   and F.f_path     = ?");
	}
	// setting parameters
	binds.file_find.null_resource = false;
		binds.file_find.data_resource = query.resource().get();
	binds.file_find.null_path = false;
		strncpy(binds.file_find.data_path, fileinfo.path().ascii(true,true).c_str(), 1024);
		binds.file_find.length_path = fileinfo.path().ascii(true,true).length();
	// getting data
	_stmt_execute(stmt_file_find, "file_find");
	while (_stmt_fetch(stmt_file_find, "file_find"))
	{
		if (!binds.file_find.null_id)
		{
			id.set(binds.file_find.data_id);
			changed = !binds.file_find.null_lost
			       || (binds.file_find.null_container) || (fileinfo.container() != c_flag    (binds.file_find.data_container ))
			       || (binds.file_find.null_size     ) || (fileinfo.size     () != c_unsigned(binds.file_find.data_size      ))
			       || (binds.file_find.null_ctime    ) || (fileinfo.ctime    () != c_stamp   (binds.file_find.data_ctime.year,binds.file_find.data_ctime.month,binds.file_find.data_ctime.day,binds.file_find.data_ctime.hour,binds.file_find.data_ctime.minute,binds.file_find.data_ctime.second))
			       || (binds.file_find.null_mtime    ) || (fileinfo.mtime    () != c_stamp   (binds.file_find.data_mtime.year,binds.file_find.data_mtime.month,binds.file_find.data_mtime.day,binds.file_find.data_mtime.hour,binds.file_find.data_mtime.minute,binds.file_find.data_mtime.second))
			       ;
			result = true;
		}
		else throw e_database(__FILE__,__LINE__,"Returned NULL id field.");
	}
	return result;
}

bool c_database_mysql::_file_add (c_unsigned & id, c_query query, c_fileinfo fileinfo)
{
	bool result = true;
	// if statement is not prepared yet, preparing it now
	if (!stmt_file_add)
	{
		MYSQL_BIND pbind[7]; memset(pbind, 0, sizeof(pbind));
		BIND_LONG    (pbind[0], file_add, resource );
		BIND_STRING  (pbind[1], file_add, path     , 4096);
		BIND_STRING  (pbind[2], file_add, name     , 4096);
		BIND_LONG    (pbind[3], file_add, container);
		BIND_LONGLONG(pbind[4], file_add, size     );
		BIND_TIME    (pbind[5], file_add, ctime    );
		BIND_TIME    (pbind[6], file_add, mtime    );
		stmt_file_add = _stmt_make(pbind, NULL, "file_add", std::string() +
			"insert into t_file (f_resource, f_path, f_name, f_container, f_size, f_ctime, f_mtime, f_found, f_seen, f_lost)" +
			" values (?,?,?,?,?,?,?,now(),now(),NULL)");
	}
	// setting parameters
	binds.file_add.null_resource = false;
		binds.file_add.data_resource = query.resource().get();
	binds.file_add.null_path = false;
		strncpy(binds.file_add.data_path, fileinfo.path().ascii(true,true).c_str(), 1024);
		binds.file_add.length_path = fileinfo.path().ascii(true,true).length();
	binds.file_add.null_name = false;
		strncpy(binds.file_add.data_name, fileinfo.path().basename().c_str(), 1024);
		binds.file_add.length_name = fileinfo.path().basename().length();
	binds.file_add.null_container = false;
		binds.file_add.data_container = fileinfo.container();
	binds.file_add.null_size = false;
		binds.file_add.data_size = fileinfo.size().get();
	binds.file_add.null_ctime = false;
		binds.file_add.data_ctime.year   = fileinfo.ctime().get_year();
		binds.file_add.data_ctime.month  = fileinfo.ctime().get_month();
		binds.file_add.data_ctime.day    = fileinfo.ctime().get_day();
		binds.file_add.data_ctime.hour   = fileinfo.ctime().get_hour();
		binds.file_add.data_ctime.minute = fileinfo.ctime().get_minute();
		binds.file_add.data_ctime.second = fileinfo.ctime().get_second();
	binds.file_add.null_mtime = false;
		binds.file_add.data_mtime.year   = fileinfo.mtime().get_year();
		binds.file_add.data_mtime.month  = fileinfo.mtime().get_month();
		binds.file_add.data_mtime.day    = fileinfo.mtime().get_day();
		binds.file_add.data_mtime.hour   = fileinfo.mtime().get_hour();
		binds.file_add.data_mtime.minute = fileinfo.mtime().get_minute();
		binds.file_add.data_mtime.second = fileinfo.mtime().get_second();
	// getting data
	try { _stmt_execute(stmt_file_add, "file_add"); }
	catch (e_database) { result = false; }
	if (result)
	{
		id.set(mysql_stmt_insert_id(stmt_file_add));
	}
	return result;
}

void c_database_mysql::_file_index (c_unsigned id, c_fileinfo fileinfo)
{
	// if statement is not prepared yet, preparing it now
	if (!stmt_file_index)
	{
		MYSQL_BIND pbind[2]; memset(pbind, 0, sizeof(pbind));
		BIND_LONG    (pbind[0], file_index, id  );
		BIND_STRING  (pbind[1], file_index, key, 4096);
		stmt_file_index = _stmt_make(pbind, NULL, "file_index", std::string() +
			"insert into t_file_index (f_file, f_key)" +
			" values (?,?)");
	}
	//
	c_string::t_value key = fileinfo.path().basename();
	while (!key.empty())
	{
	// setting parameters
	binds.file_index.null_id = false;
		binds.file_index.data_id = id.get();
	binds.file_index.null_key = false;
		strncpy(binds.file_index.data_key, key.c_str(), 1024);
		binds.file_index.length_key = key.length();
	// executing statement
	_stmt_execute(stmt_file_index, "file_index");
	// cutting key
	key.erase(0,1);
	}
}	

void c_database_mysql::_file_loose (c_query query)
{
	// if statement is not prepared yet, preparing it now
	if (!stmt_file_loose)
	{
		MYSQL_BIND pbind[2]; memset(pbind, 0, sizeof(pbind));
		BIND_TIME(pbind[0], file_loose, seen    );
		BIND_LONG(pbind[1], file_loose, resource);
		stmt_file_loose = _stmt_make(pbind, NULL, "file_loose", std::string() +
			"update t_file" +
			"   set f_lost = now()" +
			" where f_lost is null" +
			"   and f_seen < ?" +
			"   and f_resource = ?");
	}
	// setting parameters
	binds.file_loose.null_seen = false;
		binds.file_loose.data_seen.year   = f_startup.get_year();
		binds.file_loose.data_seen.month  = f_startup.get_month();
		binds.file_loose.data_seen.day    = f_startup.get_day();
		binds.file_loose.data_seen.hour   = f_startup.get_hour();
		binds.file_loose.data_seen.minute = f_startup.get_minute();
		binds.file_loose.data_seen.second = f_startup.get_second();
	binds.file_loose.null_resource = false;
		binds.file_loose.data_resource = query.resource().get();
	// getting data
	_stmt_execute(stmt_file_loose, "file_loose");
}

void c_database_mysql::_file_touch (std::vector<c_unsigned> ids)
{
	if (ids.empty()) return;
	// if statement is not prepared yet, preparing it now
	if (!stmt_file_touch)
	{
		MYSQL_BIND pbind[1]; memset(pbind, 0, sizeof(pbind));
		BIND_LONG(pbind[0], file_touch, id);
		stmt_file_touch = _stmt_make(pbind, NULL, "file_touch", std::string() +
			"update t_file" +
			"   set f_seen = now()" +
			"     , f_lost = NULL"
			" where f_file = ?");
	}
	for (std::vector<c_unsigned>::const_iterator i = ids.begin(); i != ids.end(); i++)
	{
	// setting parameters
	binds.file_touch.null_id = false;
		binds.file_touch.data_id = i->get();
	// getting data
	_stmt_execute(stmt_file_touch, "file_touch");
	}
}

void c_database_mysql::_file_flush (bool forced)
{
	if ((_file_cache.size() >= default_database_cachesize_file) || forced)
	{
		_file_touch(_file_cache);
		_file_cache.clear();
	}
}

void c_database_mysql::_file_change (c_unsigned id, c_fileinfo fileinfo)
{
	// if statement is not prepared yet, preparing it now
	if (!stmt_file_change)
	{
		MYSQL_BIND pbind[5]; memset(pbind, 0, sizeof(pbind));
		BIND_LONG    (pbind[0], file_change, container);
		BIND_LONGLONG(pbind[1], file_change, size);
		BIND_TIME    (pbind[2], file_change, ctime   );
		BIND_TIME    (pbind[3], file_change, mtime   );
		BIND_LONG    (pbind[4], file_change, id      );
		stmt_file_change = _stmt_make(pbind, NULL, "file_change", std::string() +
			"update t_file" +
			"   set f_container  = ?" +
			"     , f_size       = ?" +
			"     , f_ctime      = ?" +
			"     , f_mtime      = ?" +
			" where f_file = ?");
	}
	// setting parameters
	binds.file_change.null_container = false;
		binds.file_change.data_container = fileinfo.container().get();
	binds.file_change.null_size = false;
		binds.file_change.data_size = fileinfo.size().get();
	binds.file_change.null_ctime = false;
		binds.file_change.data_ctime.year   = fileinfo.ctime().get_year();
		binds.file_change.data_ctime.month  = fileinfo.ctime().get_month();
		binds.file_change.data_ctime.day    = fileinfo.ctime().get_day();
		binds.file_change.data_ctime.hour   = fileinfo.ctime().get_hour();
		binds.file_change.data_ctime.minute = fileinfo.ctime().get_minute();
		binds.file_change.data_ctime.second = fileinfo.ctime().get_second();
	binds.file_change.null_mtime = false;
		binds.file_change.data_mtime.year   = fileinfo.mtime().get_year();
		binds.file_change.data_mtime.month  = fileinfo.mtime().get_month();
		binds.file_change.data_mtime.day    = fileinfo.mtime().get_day();
		binds.file_change.data_mtime.hour   = fileinfo.mtime().get_hour();
		binds.file_change.data_mtime.minute = fileinfo.mtime().get_minute();
		binds.file_change.data_mtime.second = fileinfo.mtime().get_second();
	binds.file_change.null_id = false;
		binds.file_change.data_id = id.get();
	// getting data
	_stmt_execute(stmt_file_change, "file_change");
}


/***************************************************************************************************
                               FILE & SHARE REPORTING/FLUSHING ROUTINES
 ***************************************************************************************************/
c_unsigned c_database_mysql::report_share (c_query query, c_string share)
{
	// гарантируем наличие нужного хоста с обновленными данными об обнаружении и получем его id
	c_unsigned resourceid; bool changed = false;
	if (!_resource_find(resourceid, changed, query, share)) // пытаемся найти существующий
	{
		if (!_resource_add(resourceid, query, share)) // если не найден, пытаемся создать его
		{
			if(!_resource_find(resourceid, changed, query, share)) // если не создался, предполагаем что он успел создаться параллельно, и пытаемся найти снова
			{
				throw e_database(__FILE__,__LINE__,"Can not find or create resource record."); // финиш. не существует и не создается.
			} _resource_cache.push_back(resourceid);
		} else _resource_index(resourceid, share);
	} _resource_cache.push_back(resourceid);
//???	if (changed) _resource_change(resourceid); // do not need this - there is no data that can change.
	_resource_flush(false);
	return resourceid;
}

void c_database_mysql::flush_shares (c_query query)
{
	_resource_flush(true);
	_resource_loose(query);
	_resource_loosf(query);
}



c_unsigned c_database_mysql::report_file (c_query query, c_fileinfo fileinfo)
{
	// гарантируем создание записи о файле или находим существующую
	c_unsigned fileid; bool changed = false;
	if (!_file_find(fileid, changed, query, fileinfo)) // пытаемся найти существующий
	{
		if (!_file_add(fileid, query, fileinfo)) // если не найден, пытаемся создать его
		{
			if(!_file_find(fileid, changed, query, fileinfo)) // если не создался, предполагаем что он успел создаться параллельно, и пытаемся найти снова
			{
				throw e_database(__FILE__,__LINE__,"Can not find or create file record."); // финиш. не существует и не создается.
			} else _file_cache.push_back(fileid);
		} else _file_index(fileid, fileinfo);
	} else _file_cache.push_back(fileid);
	if (changed) _file_change(fileid, fileinfo);
	_file_flush(false);
	return fileid;
}


void c_database_mysql::flush_files (c_query query)
{
	_file_flush(true);
	_file_loose(query);
}
