#include "database_mysql.h"
#include "options.h"
#include "convert.h"
#include "io.h"

t_pident c_database_mysql::_initialized_pid = NULL;

c_database_mysql::c_database_mysql (string host, string user, string pass, string db, unsigned int port, string socket, unsigned long flags)
	: c_database ()
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
			throw e_database("Can not initialize mysql library for thread.", mysql_error(&handle));
		}
		_initialized_pid = getpid();
	}
	// initializing database handle
	if (mysql_init(&handle) == NULL)
	{
		throw e_database("Can not initialize mysql library.");
	}
	// reading configs
	if (mysql_options(&handle, MYSQL_READ_DEFAULT_GROUP, "filesearch"))
	{
		throw e_database("Can not set up mysql client options.", mysql_error(&handle));
	}
	// connecting
	if (mysql_real_connect(&handle, host.empty()?NULL:host.c_str(), user.empty()?NULL:user.c_str(), pass.empty()?NULL:pass.c_str(), db.empty()?NULL:db.c_str(), port, socket.empty()?NULL:socket.c_str(), flags) == NULL)
	{
		throw e_database("Can not connect to mysql database.", mysql_error(&handle));
	}
}

c_database_mysql::~c_database_mysql ()
{
	mysql_close(&handle);
}

c_database * c_database_mysql::duplicate ()
{
	c_database_mysql * result = new c_database_mysql(f_host, f_user, f_pass, f_db, f_port, f_socket, f_flags);
	result->status_init(f_process, f_startup);
	return result;
}



string c_database_mysql::escape (string s)
{
	string::size_type len = s.length();
	string::value_type * buffer = new string::value_type[len*2+1];
	unsigned long l = mysql_real_escape_string(&handle, buffer, s.c_str(), len);
	buffer[l] = 0;
	string result = buffer;
	delete[] buffer;
	return result;
}

string c_database_mysql::quote (string s)
{
	return string() + "'" + escape(s) + "'";
}

string c_database_mysql::stamp (t_time value)
{
	string result;
	result += 
}



/***************************************************************************************************
                                     DATA FETCHING ROUTINES
 ***************************************************************************************************/
c_requests c_database_mysql::fetch_requests ()
{
	c_requests result;

	string query = string() +
		"select f_filesearch_request, f_protocol, f_isnetwork, f_address, f_netmask, f_port, f_share, f_username, f_password, f_workgroup, f_selfname, f_timeout, f_depth" +
		"  from t_filesearch_request " +
		" where f_active = 1 " +
		" order by f_share desc, f_netmask desc";
	if (mysql_query(&handle, query.c_str()))
	{
		throw e_database("Can not retrieve address requests.", mysql_error(&handle));
	}

	MYSQL_RES * set = mysql_store_result(&handle);
	if (!set)
	{
		throw e_database("request retrieval failed with null set.", mysql_error(&handle));
	}

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(set)))
	{
		c_request request(
			row[ 0]?row[ 0]:"",
			row[ 1]?row[ 1]:"",
			row[ 2]?row[ 2]:"",
			row[ 3]?row[ 3]:"",
			row[ 4]?row[ 4]:"",
			row[ 5]?row[ 5]:"",
			row[ 6]?row[ 6]:"",
			row[ 7]?row[ 7]:"",
			row[ 8]?row[ 8]:"",
			row[ 9]?row[ 9]:"",
			row[10]?row[10]:"",
			row[11]?row[11]:"",
			row[12]?row[12]:"");
		result.push_back(request);
	}

	mysql_free_result(set);
	return result;
}

t_time c_database_mysql::fetch_startup ()
{
	t_time result = 0;

	string query = string() +
		"select unix_timestamp(now())";
	if (mysql_query(&handle, query.c_str()))
	{
		throw e_database("Can not retrieve current timestamp.", mysql_error(&handle));
	}

	MYSQL_RES * set = mysql_store_result(&handle);
	if (!set)
	{
		throw e_database("Current timestamp retrieval failed with null set.", mysql_error(&handle));
	}

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(set)))
	{
		result = convert::str2time(row[0]?row[0]:"");
	}

	mysql_free_result(set);
	return result;
}



/***************************************************************************************************
                                STATUS OF SCAN PROCESS ROUTINES
 ***************************************************************************************************/
bool c_database_mysql::status_check (c_request request)
{
	bool result = false;

	string query = string() +
		"select count(*) from t_filesearch_status" +
		" where f_startup  = " + convert::  time2system(f_startup) +
		"   and f_process  = " + convert::pident2system(f_process) +
		"   and f_address  = " + convert::ipaddr2system(request.address()) +
		"   and f_protocol = " + convert:: proto2system(request.proto()) +
		"   and f_port     = " + convert::ipport2system(request.port()) +
		"   and f_share    = " + quote(request.share()) +
		"   and f_username = " + quote(request.username());
	if (mysql_query(&handle, query.c_str()))
	{
		throw e_database("Can not retrieve temporary status.", mysql_error(&handle));
	}

	MYSQL_RES * set = mysql_store_result(&handle);
	if (!set)
	{
		throw e_database("Temporary status retrieval failed with null set.", mysql_error(&handle));
	}

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(set)))
	{
		result = result || (convert::str2ui(row[0]?row[0]:"") > 0);
	}

	mysql_free_result(set);
	return result;
}

void c_database_mysql::status_renew (c_request request)
{
	string query = string() +
		"insert into t_filesearch_status (                         f_startup     ,                           f_process     ,                                 f_address       ,                             f_protocol       ,                                 f_port       ,                f_share       ,                f_username       )" +
		"                         values (" + convert::time2system(f_startup) + "," + convert::pident2system(f_process) + "," + convert::ipaddr2system(request.address()) + "," + convert::proto2system(request.proto()) + "," + convert::ipport2system(request.port()) + "," + quote(request.share()) + "," + quote(request.username()) + ")";
	if (mysql_query(&handle, query.c_str()))
	{
//		throw e_database("Can not insert temporary record.", mysql_error(&handle));
	}
}

void c_database_mysql::status_clean ()
{
	string query = string() +
		"delete from t_filesearch_status" +
		" where f_startup  = " + convert::  time2system(f_startup) + 
		"   and f_process  = " + convert::pident2system(f_process) ;
	if (mysql_query(&handle, query.c_str()))
	{
		throw e_database("Can not clean temporary records.", mysql_error(&handle));
	}
}



/***************************************************************************************************
                           SUPPLIMENTARY ROUTES FOR FILE REPORTING/FLUSHING
 ***************************************************************************************************/
bool c_database_mysql::_resource_find (t_sqlid &id, bool &changed, c_request request, string share)
{
	bool result = false;
	string query = string() +
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
		throw e_database("Can not retrieve information about required resource.", mysql_error(&handle));
	} else {
		MYSQL_RES * set = mysql_store_result(&handle);
		if (!set)
		{
			DEBUG("Query ok, but result set is NULL.");
			throw e_database("Resource information retrieval failed with null set.", mysql_error(&handle));
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
	}
	return result;
}

bool c_database_mysql::_resource_add (t_sqlid &id, c_request request, string share)
{
	bool result = false;
	string query = string() +
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
	}
	return result;
}

void c_database_mysql::_resource_loose (c_request request)
{
	string query = string() +
		"update t_filesearch_resource" +
		"   set f_stamp_lost = now()" +
		" where f_address = " + convert::ipaddr2system(request.address()) +
		"   and f_stamp_seen < from_unixtime(" + convert::time2system(f_startup) + ")" +
		"   and f_stamp_lost is null";
	DEBUG("Trying query '" + query + "'.");
	if (mysql_query(&handle, query.c_str()))
	{
		DEBUG("Query failed.");
		throw e_database("Can not flush information about resources.", mysql_error(&handle));
	} else {
		DEBUG("Query successful.");
	}
}

void c_database_mysql::_resource_loosf (c_request request)
{
	string query = string() +
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
		throw e_database("Can not flush information about resources.", mysql_error(&handle));
	} else {
		DEBUG("Query successful.");
	}
}

void c_database_mysql::_resource_touch (vector<t_sqlid> ids)
{
	if (ids.empty()) return;
	string ids_s;
	for (vector<t_sqlid>::const_iterator i = ids.begin(); i != ids.end(); i++)
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
		throw e_database("Can not update information about resource.", mysql_error(&handle));
	} else {
		DEBUG("Query successful.");
	}
}

void c_database_mysql::_resource_flush (bool forced)
{
	if ((_resource_cache.size() >= options::database_cachesize_resource) || forced)
	{
		_resource_touch(_resource_cache);
		_resource_cache.clear();
	}
}



bool c_database_mysql::_file_find (t_sqlid &id, bool &changed, c_request request, c_fileinfo fileinfo)
{
	bool result = false;
	string query = string() +
		"select f_filesearch_file,t_filesearch_file.f_stamp_lost,f_container,f_size,unix_timestamp(f_ctime),unix_timestamp(f_mtime) from t_filesearch_file, t_filesearch_resource" +
		" where t_filesearch_file.f_filesearch_resource = t_filesearch_resource.f_filesearch_resource" +
		"   and t_filesearch_file.f_filesearch_resource = " + convert::sqlid2system(request.resourceid()) +
		"   and f_path   = " + quote(convert::path2print(fileinfo.path()));
	DEBUG("Trying query '" + query + "'.");
	if (mysql_query(&handle, query.c_str()))
	{
		DEBUG("Query failed.");
		throw e_database("Can not retrieve information about requested file.", mysql_error(&handle));
	} else {
		MYSQL_RES * set = mysql_store_result(&handle);
		if (!set)
		{
			DEBUG("Query ok, but result set is NULL.");
			throw e_database("File information retrieval failed with null set.", mysql_error(&handle));
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
	}
	return result;
}

bool c_database_mysql::_file_add (t_sqlid &id, c_request request, c_fileinfo fileinfo)
{
	bool result = false;
	string query = string() +
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
	}
	return result;
}

void c_database_mysql::_file_loose (c_request request)
{
	string query = string() +
		"update t_filesearch_file" +
		"   set f_stamp_lost = now()" +
		" where f_filesearch_resource = " + convert::sqlid2system(request.resourceid()) +
		"   and f_stamp_seen < from_unixtime(" + convert::time2system(f_startup) + ")" +
		"   and f_stamp_lost is null";
	DEBUG("Trying query '" + query + "'.");
	if (mysql_query(&handle, query.c_str()))
	{
		DEBUG("Query failed.");
		throw e_database("Can not flush information about files.", mysql_error(&handle));
	} else {
		DEBUG("Query successful.");
	}
}

void c_database_mysql::_file_touch (vector<t_sqlid> ids)
{
	if (ids.empty()) return;
	string ids_s;
	for (vector<t_sqlid>::const_iterator i = ids.begin(); i != ids.end(); i++)
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
		throw e_database("Can not update information about files.", mysql_error(&handle));
	} else {
		DEBUG("Query successful.");
	}
}

void c_database_mysql::_file_flush (bool forced)
{
	if ((_file_cache.size() >= options::database_cachesize_file) || forced)
	{
		_file_touch(_file_cache);
		_file_cache.clear();
	}
}

void c_database_mysql::_file_change (t_sqlid id, c_fileinfo fileinfo)
{
	string query = string() +
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
		throw e_database("Can not update information about file.", mysql_error(&handle));
	} else {
		DEBUG("Query successful.");
	}
}



/***************************************************************************************************
                               FILE & SHARE REPORTING/FLUSHING ROUTINES
 ***************************************************************************************************/
t_sqlid c_database_mysql::report_share (c_request request, string share)
{
	// гарантируем наличие нужного хоста с обновленными данными об обнаружении и получем его id
	t_sqlid resourceid; bool changed = false;
	if (!_resource_find(resourceid, changed, request, share)) // пытаемся найти существующий
	{
		if (!_resource_add(resourceid, request, share)) // если не найден, пытаемся создать его
		{
			if(!_resource_find(resourceid, changed, request, share)) // если не создался, предполагаем что он успел создаться параллельно, и пытаемся найти снова
			{
				throw e_database("Can not find or create resource record."); // финиш. не существует и не создается.
			}
		}
	}
//???	if (changed) _resource_change(resourceid); // do not need this - there is no data that can change.
	_resource_cache.push_back(resourceid);
	_resource_flush(false);
	return resourceid;
}

void c_database_mysql::flush_shares (c_request request)
{
	_resource_flush(true);
	_resource_loose(request);
	_resource_loosf(request);
}



t_sqlid c_database_mysql::report_file (c_request request, c_fileinfo fileinfo)
{
	// гарантируем создание записи о файле или находим существующую
	t_sqlid fileid; bool changed = false;
	if (!_file_find(fileid, changed, request, fileinfo)) // пытаемся найти существующий
	{
		if (!_file_add(fileid, request, fileinfo)) // если не найден, пытаемся создать его
		{
			if(!_file_find(fileid, changed, request, fileinfo)) // если не создался, предполагаем что он успел создаться параллельно, и пытаемся найти снова
			{
				throw e_database("Can not find or create file record."); // финиш. не существует и не создается.
			}
		}
	}
	if (changed) _file_change(fileid, fileinfo);
	_file_cache.push_back(fileid);
	_file_flush(false);
	return fileid;
}


void c_database_mysql::flush_files (c_request request)
{
	_file_flush(true);
	_file_loose(request);
}
