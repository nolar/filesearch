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

#define BIND_TIME(BIND,STMTNAME,FIELDNAME) \
	BIND.buffer_type = MYSQL_TYPE_DATETIME; \
	BIND.buffer = &binds.STMTNAME.data_##FIELDNAME; \
	BIND.is_null = &binds.STMTNAME.null_##FIELDNAME;

pid_t c_database_mysql::_initialized_pid = 0;

c_database_mysql::c_database_mysql (c_string host, c_string user, c_string pass, c_string db, c_unsigned port, c_string socket, c_unsigned flags)
	: c_database ()
	, stmt_requests    (NULL)
	, stmt_startup     (NULL)
	, stmt_status_check(NULL)
	, stmt_status_renew(NULL)
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
	if (stmt_requests    ) mysql_stmt_close(stmt_requests    );
	if (stmt_startup     ) mysql_stmt_close(stmt_startup     );
	if (stmt_status_check) mysql_stmt_close(stmt_status_check);
	if (stmt_status_renew) mysql_stmt_close(stmt_status_renew);
	mysql_close(&handle);
}

c_database * c_database_mysql::duplicate ()
{
	c_database_mysql * result = new c_database_mysql(f_host, f_user, f_pass, f_db, f_port, f_socket, f_flags);
	result->status_init(f_process, f_startup);
	return result;
}



c_string c_database_mysql::escape (c_string s)
{
	// !!! use auto_ptr here for buffer
	c_string::t_value::size_type len = s.length();
	std::auto_ptr<c_string::t_value::value_type> buffer(new c_string::t_value::value_type[len*2+1]);
	unsigned long l = mysql_real_escape_string(&handle, buffer.get(), s.c_str(), len);
	buffer.get()[l] = 0;
	return buffer.get();
}

c_string c_database_mysql::quote (c_string s)
{
	return c_string() + "'" + escape(s) + "'";
}

//string c_database_mysql::stamp (t_time value)
//{
//	string result;
//	result += 
//}

MYSQL_STMT * c_database_mysql::_stmt_make (MYSQL_BIND * params, MYSQL_BIND * results, std::string query)
{
	// allocating statement for current connection
	MYSQL_STMT * stmt = mysql_stmt_init(&handle);
	if (!stmt)
	{
		throw e_database(__FILE__,__LINE__,"Can not initialize statement.", mysql_error(&handle));
	}
	// parsing query for statement
	if (mysql_stmt_prepare(stmt, query.c_str(), query.length()))
	{
		std::string msg = mysql_stmt_error(stmt);
		mysql_stmt_close(stmt);
		throw e_database(__FILE__,__LINE__,"Can not prepare statement.", msg);
	}
	// binding fields with statement
	if (params)
		if (mysql_stmt_bind_param(stmt, params))
		{
			std::string msg = mysql_stmt_error(stmt);
			mysql_stmt_close(stmt);
			throw e_database(__FILE__,__LINE__,"Can not bind parameters to statement.", msg);
		}
	// binding fields with statement
	if (results)
		if (mysql_stmt_bind_result(stmt, results))
		{
			std::string msg = mysql_stmt_error(stmt);
			mysql_stmt_close(stmt);
			throw e_database(__FILE__,__LINE__,"Can not bind results to statement.", msg);
		}
	// return gotten statement
	return stmt;
}

void c_database_mysql::_stmt_execute (MYSQL_STMT * stmt)
{
	if (mysql_stmt_execute(stmt))
	{
		std::string msg = mysql_stmt_error(stmt);
		throw e_database(__FILE__,__LINE__,"Can not execute prepared statement.", msg);
	}
}

bool c_database_mysql::_stmt_fetch (MYSQL_STMT * stmt)
{
	if (!stmt) throw e_database(__FILE__,__LINE__,"Can not fetch from NULL statement.");
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
		throw e_database(__FILE__,__LINE__,"Error while fetching from statement.", msg);
	}
	else return true;
}




/***************************************************************************************************
                                     DATA FETCHING ROUTINES
 ***************************************************************************************************/
c_requests c_database_mysql::fetch_requests ()
{
	c_requests result;
	// if statement is not prepared yet, preparing it now
	if (!stmt_requests)
	{
		MYSQL_BIND rbind[13]; memset(rbind, 0, sizeof(rbind));
		BIND_LONG  (rbind[ 0], requests, id       );
		BIND_LONG  (rbind[ 1], requests, protocol );
		BIND_LONG  (rbind[ 2], requests, isnetwork);
		BIND_STRING(rbind[ 3], requests, ipaddr   , 1024);
		BIND_LONG  (rbind[ 4], requests, netmask  );
		BIND_LONG  (rbind[ 5], requests, ipport   );
		BIND_STRING(rbind[ 6], requests, share    , 1024);
		BIND_STRING(rbind[ 7], requests, username , 1024);
		BIND_STRING(rbind[ 8], requests, password , 1024);
		BIND_STRING(rbind[ 9], requests, workgroup, 1024);
		BIND_STRING(rbind[10], requests, selfname , 1024);
		BIND_LONG  (rbind[11], requests, timeout  );
		BIND_LONG  (rbind[12], requests, depth    );
		stmt_requests = _stmt_make(NULL, rbind, std::string() +
			"select f_filesearch_request, f_protocol, f_isnetwork, f_address, f_netmask, f_port, f_share, f_username, f_password, f_workgroup, f_selfname, f_timeout, f_depth" +
			"  from t_filesearch_request " +
			" where f_active = 1 " +
			" order by f_share desc, f_netmask desc");
	}
	// getting data
	_stmt_execute(stmt_requests);
	while (_stmt_fetch(stmt_requests))
	{
		c_unsigned o_id        ; if (!binds.requests.null_id       ) o_id        = binds.requests.data_id        ; else throw e_database(__FILE__,__LINE__,"Identifier field of request is NULL.");
		c_protocol o_protocol  ; if (!binds.requests.null_protocol ) o_protocol  = binds.requests.data_protocol  ; else throw e_database(__FILE__,__LINE__,"Field 'protocol' is NULL." );
		c_flag     o_isnetwork ; if (!binds.requests.null_isnetwork) o_isnetwork = binds.requests.data_isnetwork ; else throw e_database(__FILE__,__LINE__,"Field 'isnetwork' is NULL.");
		c_ipaddr   o_ipaddr    ; if (!binds.requests.null_ipaddr   ) o_ipaddr    = binds.requests.null_netmask ? c_ipaddr(std::string(binds.requests.data_ipaddr, binds.requests.length_ipaddr)) : c_ipaddr(std::string(binds.requests.data_ipaddr, binds.requests.length_ipaddr), binds.requests.data_netmask); else throw e_database(__FILE__,__LINE__,"Field 'ipaddr' is NULL.");
		c_unsigned o_ipport    ; if (!binds.requests.null_ipport   ) o_ipport    = binds.requests.data_ipport    ;
		c_string   o_share     ; if (!binds.requests.null_share    ) o_share     = std::string(binds.requests.data_share     , binds.requests.length_share    );
		c_string   o_username  ; if (!binds.requests.null_username ) o_username  = std::string(binds.requests.data_username  , binds.requests.length_username );
		c_string   o_password  ; if (!binds.requests.null_password ) o_password  = std::string(binds.requests.data_password  , binds.requests.length_password );
		c_string   o_workgroup ; if (!binds.requests.null_workgroup) o_workgroup = std::string(binds.requests.data_workgroup , binds.requests.length_workgroup);
		c_string   o_selfname  ; if (!binds.requests.null_selfname ) o_selfname  = std::string(binds.requests.data_selfname  , binds.requests.length_selfname );
		c_unsigned o_timeout   ; if (!binds.requests.null_timeout  ) o_timeout   = binds.requests.data_timeout   ;
		c_unsigned o_depth     ; if (!binds.requests.null_depth    ) o_depth     = binds.requests.data_depth     ; else o_depth = default_request_depth;
		result.push_back(c_request(o_id, o_protocol, o_isnetwork, o_ipaddr, o_ipport, o_share, o_username, o_password, o_workgroup, o_selfname, o_timeout, o_depth));
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
		stmt_startup = _stmt_make(NULL, rbind, std::string() +
			"select now()");
	}
	// getting data
	_stmt_execute(stmt_startup);
	while (_stmt_fetch(stmt_startup))
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
bool c_database_mysql::status_check (c_request request)
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
		stmt_startup = _stmt_make(pbind, rbind, std::string() +
			"select count(*) from t_filesearch_status" +
			" where f_startup  = ?" + 
			"   and f_process  = ?" + 
			"   and f_protocol = ?" + 
			"   and f_address  = ?" + 
			"   and f_port     = ?" + 
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
	binds.status_check.null_ipaddr = false;
		strncpy(binds.status_check.data_ipaddr, request.ipaddr().ascii(-1).c_str(), 1024);
		binds.status_check.length_ipaddr = request.ipaddr().ascii(-1).length();
	binds.status_check.null_protocol = false;
		binds.status_check.data_protocol = request.protocol().get();
	binds.status_check.null_ipport = false;
		binds.status_check.data_ipport = request.ipport().get();
	binds.status_check.null_share = false;
		strncpy(binds.status_check.data_share, request.share().c_str(), 1024);
		binds.status_check.length_share = request.share().length();
	binds.status_check.null_username = false;
		strncpy(binds.status_check.data_username, request.username().c_str(), 1024);
		binds.status_check.length_username = request.username().length();
	// getting data
	_stmt_execute(stmt_startup);
	while (_stmt_fetch(stmt_startup))
	{
		if (!binds.status_check.null_count)
		{
			if (binds.status_check.data_count)
				result = true;
		}
		else throw e_database(__FILE__,__LINE__,"Startup field is NULL.");
	}
	return result;
}

void c_database_mysql::status_renew (c_request request)
{
/*	string query = string() +
		"insert into t_filesearch_status (                         f_startup     ,                           f_process     ,                                 f_address       ,                             f_protocol       ,                                 f_port       ,                f_share       ,                f_username       )" +
		"                         values (" + convert::time2system(f_startup) + "," + convert::pident2system(f_process) + "," + convert::ipaddr2system(request.address()) + "," + convert::proto2system(request.proto()) + "," + convert::ipport2system(request.port()) + "," + quote(request.share()) + "," + quote(request.username()) + ")";
	if (mysql_query(&handle, query.c_str()))
	{
//		throw e_database(__FILE__,__LINE__,"Can not insert temporary record.", mysql_error(&handle));
	}*/
}

void c_database_mysql::status_clean ()
{
/*	string query = string() +
		"delete from t_filesearch_status" +
		" where f_startup  = " + convert::  time2system(f_startup) + 
		"   and f_process  = " + convert::pident2system(f_process) ;
	if (mysql_query(&handle, query.c_str()))
	{
		throw e_database(__FILE__,__LINE__,"Can not clean temporary records.", mysql_error(&handle));
	}*/
}



/***************************************************************************************************
                           SUPPLIMENTARY ROUTES FOR FILE REPORTING/FLUSHING
 ***************************************************************************************************/
bool c_database_mysql::_resource_find (c_unsigned & id, bool & changed, c_request request, c_string share)
{
	bool result = false;
/*	string query = string() +
		"select f_filesearch_resource,t_filesearch_resource.f_stamp_lost from t_filesearch_resource, t_filesearch_request" +
		" where t_filesearch_request.f_filesearch_request = t_filesearch_resource.f_filesearch_request" +
		" and   t_filesearch_request .f_protocol = " + convert:: proto2system(request.proto()  ) +
		" and   t_filesearch_resource.f_address  = " + convert::ipaddr2system(request.address()) +
		" and   t_filesearch_request .f_port     = " + convert::ipport2system(request.port()   ) +
		" and   t_filesearch_resource.f_share    = " + quote(share)                              +
		" and   t_filesearch_request .f_username = " + quote(request.username())                 ;
	DEBUG("Trying query '" + query + "'.");
	if (mysql_query(&handle, query.c_str()))
	{
		DEBUG("Query failed.");
		throw e_database(__FILE__,__LINE__,"Can not retrieve information about required resource.", mysql_error(&handle));
	} else {
		MYSQL_RES * set = mysql_store_result(&handle);
		if (!set)
		{
			DEBUG("Query ok, but result set is NULL.");
			throw e_database(__FILE__,__LINE__,"Resource information retrieval failed with null set.", mysql_error(&handle));
		}
		DEBUG("Query successful.");
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(set)))
		{
			id = convert::str2sqlid(row[0]?row[0]:"");
			changed = (row[1] != NULL);
			result = true;
		}
		mysql_free_result(set);
	}*/
	return result;
}

bool c_database_mysql::_resource_add (c_unsigned & id, c_request request, c_string share)
{
	bool result = false;
/*	string query = string() +
		"insert into t_filesearch_resource (                f_filesearch_request       ,                                 f_address       ,        f_share     , f_stamp_found, f_stamp_seen, f_stamp_lost)" +
		"                           values (" + convert::sqlid2system(request.id()) + "," + convert::ipaddr2system(request.address()) + "," + quote(share) + ", now()        , now()       , NULL        )";
	DEBUG("Trying query '" + query + "'.");
	if (mysql_query(&handle, query.c_str()))
	{
		DEBUG("Query failed.");
		//??? what to do if can not add record?
	} else {
		id = mysql_insert_id(&handle);
		result = true;
		DEBUG("Query successful. Inserted id is "+convert::sqlid2print(id));
	}*/
	return result;
}

void c_database_mysql::_resource_loose (c_request request)
{
/*	string query = string() +
		"update t_filesearch_resource" +
		"   set f_stamp_lost = now()" +
		" where f_address = " + convert::ipaddr2system(request.address()) +
		"   and f_stamp_seen < from_unixtime(" + convert::time2system(f_startup) + ")" +
		"   and f_stamp_lost is null";
	DEBUG("Trying query '" + query + "'.");
	if (mysql_query(&handle, query.c_str()))
	{
		DEBUG("Query failed.");
		throw e_database(__FILE__,__LINE__,"Can not flush information about resources.", mysql_error(&handle));
	} else {
		DEBUG("Query successful.");
	}*/
}

void c_database_mysql::_resource_loosf (c_request request)
{
/*	string query = string() +
		"update t_filesearch_file" +
		"   set f_stamp_lost = now()" +
		" where f_stamp_seen < from_unixtime(" + convert::time2system(f_startup) + ")" +
		"   and f_stamp_lost is null" +
		"   and f_filesearch_resource in" +
		"           (select f_filesearch_resource from t_filesearch_resource" + 
		"             where f_address = " + convert::ipaddr2system(request.address()) +
		"               and f_stamp_lost is not null)";
	DEBUG("Trying query '" + query + "'.");
	if (mysql_query(&handle, query.c_str()))
	{
		DEBUG("Query failed.");
		throw e_database(__FILE__,__LINE__,"Can not flush information about resources.", mysql_error(&handle));
	} else {
		DEBUG("Query successful.");
	}*/
}

void c_database_mysql::_resource_touch (std::vector<c_unsigned> ids)
{
/*	if (ids.empty()) return;
	string ids_s;
	for (vector<c_unsigned>::const_iterator i = ids.begin(); i != ids.end(); i++)
		ids_s += ((i == ids.begin()) ? "" : ",") + convert::sqlid2system(*i);
	string query = string() +
		"update t_filesearch_resource" +
		"   set f_stamp_seen = now()" +
		"     , f_stamp_lost = NULL"
		" where f_filesearch_resource in (" + ids_s + ")";
	DEBUG("Trying query '" + query + "'.");
	if (mysql_query(&handle, query.c_str()))
	{
		DEBUG("Query failed.");
		throw e_database(__FILE__,__LINE__,"Can not update information about resource.", mysql_error(&handle));
	} else {
		DEBUG("Query successful.");
	}*/
}

void c_database_mysql::_resource_flush (bool forced)
{
/*	if ((_resource_cache.size() >= options::database_cachesize_resource) || forced)
	{
		_resource_touch(_resource_cache);
		_resource_cache.clear();
	}*/
}



bool c_database_mysql::_file_find (c_unsigned & id, bool & changed, c_request request, c_fileinfo fileinfo)
{
	bool result = false;
/*	string query = string() +
		"select f_filesearch_file,t_filesearch_file.f_stamp_lost,f_container,f_size,unix_timestamp(f_ctime),unix_timestamp(f_mtime) from t_filesearch_file, t_filesearch_resource" +
		" where t_filesearch_file.f_filesearch_resource = t_filesearch_resource.f_filesearch_resource" +
		"   and t_filesearch_file.f_filesearch_resource = " + convert::sqlid2system(request.resourceid()) +
		"   and f_path   = " + quote(convert::path2print(fileinfo.path()));
	DEBUG("Trying query '" + query + "'.");
	if (mysql_query(&handle, query.c_str()))
	{
		DEBUG("Query failed.");
		throw e_database(__FILE__,__LINE__,"Can not retrieve information about requested file.", mysql_error(&handle));
	} else {
		MYSQL_RES * set = mysql_store_result(&handle);
		if (!set)
		{
			DEBUG("Query ok, but result set is NULL.");
			throw e_database(__FILE__,__LINE__,"File information retrieval failed with null set.", mysql_error(&handle));
		}
		DEBUG("Query successful.");
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(set)))
		{
			id = convert::str2sqlid(row[0]?row[0]:"");
			changed =  (row[1] != NULL)
				|| (row[2] == NULL) || (convert::str2flag(row[2]) != fileinfo.container())
				|| (row[3] == NULL) || (convert::str2size(row[3]) != fileinfo.size()     )
				|| (row[4] == NULL) || (convert::str2time(row[4]) != fileinfo.ctime()    )
				|| (row[5] == NULL) || (convert::str2time(row[5]) != fileinfo.mtime()    );
			DEBUG("File found; id="+convert::sqlid2print(id)+"; changed="+convert::flag2print(changed)+".");
			result = true;
		}
		mysql_free_result(set);
	}*/
	return result;
}

bool c_database_mysql::_file_add (c_unsigned & id, c_request request, c_fileinfo fileinfo)
{
	bool result = false;
/*	string query = string() +
		"insert into t_filesearch_file (                     f_filesearch_resource         ,                                     f_path        ,                 f_name       ,                                f_container       ,                                f_size       ,                                              f_ctime        ,                                              f_mtime       , f_stamp_found, f_stamp_seen, f_stamp_lost)" +
		"                       values (" + convert::sqlid2system(request.resourceid()) + "," + quote(convert::path2print(fileinfo.path())) + "," + quote(fileinfo.name()) + "," + convert::flag2system(fileinfo.container()) + "," + convert::size2system(fileinfo.size()) + ",from_unixtime(" + convert::time2system(fileinfo.ctime()) + "),from_unixtime(" + convert::time2system(fileinfo.mtime())+ "), now()        , now()       , NULL        )";
	DEBUG("Trying query '" + query + "'.");
	if (mysql_query(&handle, query.c_str()))
	{
		DEBUG("Query failed.");
		//???!!! what to do if can not insert record?
	} else {
		id = mysql_insert_id(&handle);
		result = true;
		DEBUG("Query successful. Inserted id is "+convert::sqlid2print(id));
	}*/
	return result;
}

void c_database_mysql::_file_loose (c_request request)
{
/*	string query = string() +
		"update t_filesearch_file" +
		"   set f_stamp_lost = now()" +
		" where f_filesearch_resource = " + convert::sqlid2system(request.resourceid()) +
		"   and f_stamp_seen < from_unixtime(" + convert::time2system(f_startup) + ")" +
		"   and f_stamp_lost is null";
	DEBUG("Trying query '" + query + "'.");
	if (mysql_query(&handle, query.c_str()))
	{
		DEBUG("Query failed.");
		throw e_database(__FILE__,__LINE__,"Can not flush information about files.", mysql_error(&handle));
	} else {
		DEBUG("Query successful.");
	}*/
}

void c_database_mysql::_file_touch (std::vector<c_unsigned> ids)
{
/*	if (ids.empty()) return;
	string ids_s;
	for (vector<c_unsigned>::const_iterator i = ids.begin(); i != ids.end(); i++)
		ids_s += ((i == ids.begin()) ? "" : ",") + convert::sqlid2system(*i);
	string query = string() +
		"update t_filesearch_file" +
		"   set f_stamp_seen = now()" +
		"     , f_stamp_lost = NULL"
		" where f_filesearch_file in (" + ids_s + ")";
	DEBUG("Trying query '" + query + "'.");
	if (mysql_query(&handle, query.c_str()))
	{
		DEBUG("Query failed.");
		throw e_database(__FILE__,__LINE__,"Can not update information about files.", mysql_error(&handle));
	} else {
		DEBUG("Query successful.");
	}*/
}

void c_database_mysql::_file_flush (bool forced)
{
/*	if ((_file_cache.size() >= options::database_cachesize_file) || forced)
	{
		_file_touch(_file_cache);
		_file_cache.clear();
	}*/
}

void c_database_mysql::_file_change (c_unsigned id, c_fileinfo fileinfo)
{
/*	string query = string() +
		"update t_filesearch_file" +
		"   set f_container  = "               + convert::flag2system(fileinfo.container())      +
		"     , f_size       = "               + convert::size2system(fileinfo.size() )          +
		"     , f_ctime      = from_unixtime(" + convert::time2system(fileinfo.ctime())    + ")" +
		"     , f_mtime      = from_unixtime(" + convert::time2system(fileinfo.mtime())    + ")" +
		" where f_filesearch_file = " + convert::sqlid2system(id);
	DEBUG("Trying query '" + query + "'.");
	if (mysql_query(&handle, query.c_str()))
	{
		DEBUG("Query failed.");
		throw e_database(__FILE__,__LINE__,"Can not update information about file.", mysql_error(&handle));
	} else {
		DEBUG("Query successful.");
	}*/
}



/***************************************************************************************************
                               FILE & SHARE REPORTING/FLUSHING ROUTINES
 ***************************************************************************************************/
c_unsigned c_database_mysql::report_share (c_request request, c_string share)
{
	LOG("SHARE '"+share.ascii()+"'.");
/*	// гарантируем наличие нужного хоста с обновленными данными об обнаружении и получем его id
	c_unsigned resourceid; bool changed = false;
	if (!_resource_find(resourceid, changed, request, share)) // пытаемся найти существующий
	{
		if (!_resource_add(resourceid, request, share)) // если не найден, пытаемся создать его
		{
			if(!_resource_find(resourceid, changed, request, share)) // если не создался, предполагаем что он успел создаться параллельно, и пытаемся найти снова
			{
				throw e_database(__FILE__,__LINE__,"Can not find or create resource record."); // финиш. не существует и не создается.
			}
		}
	}
//???	if (changed) _resource_change(resourceid); // do not need this - there is no data that can change.
	_resource_cache.push_back(resourceid);
	_resource_flush(false);
	return resourceid;*/
}

void c_database_mysql::flush_shares (c_request request)
{
	LOG("SHARE FLUSH.");
/*	_resource_flush(true);
	_resource_loose(request);
	_resource_loosf(request);*/
}



c_unsigned c_database_mysql::report_file (c_request request, c_fileinfo fileinfo)
{
	LOG((fileinfo.container()?"DIR":"FILE")+" '"+fileinfo.path().ascii(true,true)+"'.");
/*	// гарантируем создание записи о файле или находим существующую
	c_unsigned fileid; bool changed = false;
	if (!_file_find(fileid, changed, request, fileinfo)) // пытаемся найти существующий
	{
		if (!_file_add(fileid, request, fileinfo)) // если не найден, пытаемся создать его
		{
			if(!_file_find(fileid, changed, request, fileinfo)) // если не создался, предполагаем что он успел создаться параллельно, и пытаемся найти снова
			{
				throw e_database(__FILE__,__LINE__,"Can not find or create file record."); // финиш. не существует и не создается.
			}
		}
	}
	if (changed) _file_change(fileid, fileinfo);
	_file_cache.push_back(fileid);
	_file_flush(false);
	return fileid;*/
}


void c_database_mysql::flush_files (c_request request)
{
	LOG("FILE FLUSH.");
/*	_file_flush(true);
	_file_loose(request);*/
}
