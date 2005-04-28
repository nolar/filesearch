#include "database_mysql.h"
#include <iostream>
#include <iomanip>

pid_t c_database_mysql::_initialized_pid = NULL;

c_database_mysql::c_database_mysql (pid_t a_process, time_t a_startup, string host, string user, string pass, string db, unsigned int port, string socket, unsigned long flags)
	: c_database (a_process, a_startup)
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
	c_database_mysql * result = new c_database_mysql(f_process, f_startup, f_host, f_user, f_pass, f_db, f_port, f_socket, f_flags);
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



/***************************************************************************************************
                                STATUS OF SCAN PROCESS ROUTINES
 ***************************************************************************************************/
bool c_database_mysql::status_check (c_request request)
{
	bool result = false;

	string query = string() +
		"select count(*) from t_filesearch_status" +
		" where f_startup  = '" + escape(utils::ultostr(f_startup)) + "'" +
		"   and f_process  = '" + escape(utils::ultostr(f_process)) + "'" +
		"   and f_address  = '" + escape(utils::ultostr(request.address())) + "'" +
		"   and f_protocol = '" + escape(utils::ultostr(request.proto())) + "'" +
		"   and f_port     = '" + escape(utils::ultostr(request.port())) + "'" +
		"   and f_share    = '" + escape(request.share()) + "'" +
		"   and f_username = '" + escape(request.username()) + "'";
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
		result = result || (utils::strtoul(row[0]?row[0]:"") > 0);
	}

	mysql_free_result(set);
	return result;
}

void c_database_mysql::status_renew (c_request request)
{
	string query = string() +
		"insert into t_filesearch_status (f_startup, f_process, f_address, f_protocol, f_port, f_share, f_username) values " +
		"('" + escape(utils::ultostr(f_startup)) + "','" + escape(utils::ultostr(f_process)) + "','" + escape(utils::ultostr(request.address()))+ "','" + escape(utils::ultostr(request.proto()))+ "','" + escape(utils::ultostr(request.port()))+ "','" + escape(request.share()) + "','" + escape(request.username()) + "')";
	if (mysql_query(&handle, query.c_str()))
	{
//		throw e_database("Can not insert temporary record.", mysql_error(&handle));
	}
}

void c_database_mysql::status_clean ()
{
	string query = string() +
		"delete from t_filesearch_status" +
		" where f_startup  = '" + escape(utils::ultostr(f_startup)) + "'" +
		"   and f_process  = '" + escape(utils::ultostr(f_process)) + "'";
//	if (mysql_query(&handle, query.c_str()))
//	{
//		throw e_database("Can not clean temporary records.", mysql_error(&handle));
//	}
}



/***************************************************************************************************
                           SUPPLIMENTARY ROUTES FOR FILE REPORTING/FLUSHING
 ***************************************************************************************************/
bool c_database_mysql::_select_resource (t_id &id, c_request request, string share)
{
	bool result = false;
	string query = string() +
		"select f_filesearch_resource from t_filesearch_resource, t_filesearch_request" +
		" where t_filesearch_request.f_filesearch_request = t_filesearch_resource.f_filesearch_request" +
		" and   t_filesearch_request .f_protocol = '" + escape(utils::ultostr(request.proto()))    + "'" +
		" and   t_filesearch_resource.f_address  = '" + escape(utils::ultostr(request.address()) ) + "'" +
		" and   t_filesearch_request .f_port     = '" + escape(utils::ultostr(request.port()))     + "'" +
		" and   t_filesearch_resource.f_share    = '" + escape(share)                              + "'" +
		" and   t_filesearch_request .f_username = '" + escape(request.username())                 + "'";
	if (mysql_query(&handle, query.c_str()))
	{
		throw e_database("Can not retrieve information about required resource.", mysql_error(&handle));
	} else {
		MYSQL_RES * set = mysql_store_result(&handle);
		if (!set)
		{
			throw e_database("Resource information retrieval failed with null set.", mysql_error(&handle));
		}
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(set)))
		{
			id = utils::strtoul(row[0]?row[0]:"");
			result = true;
		}
		mysql_free_result(set);
	}
	return result;
}

bool c_database_mysql::_insert_resource (t_id &id, c_request request, string share)
{
	bool result = false;
	string query = string() +
		"insert into t_filesearch_resource (f_filesearch_request, f_address, f_share, f_stamp_found)" +
		" values ('" + escape(utils::ultostr(request.id())) + "'" + 
			",'" + escape(utils::ultostr(request.address())) + "'" +
			",'" + escape(share) + "'" +
			", now())";
	if (mysql_query(&handle, query.c_str()))
	{
	} else {
		id = mysql_insert_id(&handle);
		result = true;
	}
	return result;
}

void c_database_mysql::_update_resource (t_id id)
{
	string query = string() +
		"update t_filesearch_resource" +
		"   set f_stamp_seen = now()" +
		"     , f_stamp_lost = NULL"
		" where f_filesearch_resource = '" + escape(utils::ultostr(id)) + "'";
	if (mysql_query(&handle, query.c_str()))
	{
		throw e_database("Can not update information about resource.", mysql_error(&handle));
	}
}

void c_database_mysql::_loose_resources (c_request request)
{
	string query = string() +
		"update t_filesearch_resource" +
		"   set f_stamp_lost = now()" +
		" where f_stamp_seen < from_unixtime(" + utils::ultostr(f_startup) + ")" +
		"   and f_address = '" + utils::ultostr(request.address()) + "'";
		
	if (mysql_query(&handle, query.c_str()))
	{
		throw e_database("Can not flush information about resources.", mysql_error(&handle));
	}
}



bool c_database_mysql::_select_file (t_id &id, c_request request, c_fileinfo fileinfo)
{
	bool result = false;
	string query = string() +
		"select f_filesearch_file from t_filesearch_file, t_filesearch_resource" +
		" where t_filesearch_file.f_filesearch_resource = t_filesearch_resource.f_filesearch_resource"
		"   and t_filesearch_file.f_filesearch_resource = '" + escape(utils::ultostr(request.resourceid())) + "'" +
		"   and f_path   = '" + escape(utils::path2string(fileinfo.path())) + "'" +
		"   and f_name   = '" + escape(fileinfo.name()) + "'";
	if (mysql_query(&handle, query.c_str()))
	{
		throw e_database("Can not retrieve information about requested file.", mysql_error(&handle));
	} else {
		MYSQL_RES * set = mysql_store_result(&handle);
		if (!set)
		{
			throw e_database("File information retrieval failed with null set.", mysql_error(&handle));
		}
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(set)))
		{
			id = utils::strtoul(row[0]?row[0]:"");
			result = true;
		}
		mysql_free_result(set);
	}
	return result;
	
}

bool c_database_mysql::_insert_file (t_id &id, c_request request, c_fileinfo fileinfo)
{
	bool result = false;
	string query = string() +
		"insert into t_filesearch_file (f_filesearch_resource, f_path, f_name, f_stamp_found)" +
		" values ('" + escape(utils::ultostr(request.resourceid())) + "'" +
		"        ,'" + escape(utils::path2string(fileinfo.path()))  + "'" +
		"        ,'" + escape(fileinfo.name()) + "'" +
		"        , now())";
	if (mysql_query(&handle, query.c_str()))
	{
	} else {
		id = mysql_insert_id(&handle);
		result = true;
	}
	return result;
}

void c_database_mysql::_update_file (t_id id, c_fileinfo fileinfo)
{
	string query = string() +
		"update t_filesearch_file" +
		"   set f_stamp_seen = now()" +
		"     , f_stamp_lost = NULL" + 
		"     , f_size       = '" + escape(utils::ultostr(fileinfo.size()))  + "'" +
		"     , f_ctime      = from_unixtime(" + utils::ultostr(fileinfo.ctime()) + ")" +
		"     , f_mtime      = from_unixtime(" + utils::ultostr(fileinfo.mtime()) + ")" +
		" where f_filesearch_file = '" + escape(utils::ultostr(id)) + "'";
	if (mysql_query(&handle, query.c_str()))
	{
		throw e_database("Can not update information about file.", mysql_error(&handle));
	}
}

void c_database_mysql::_loose_files (c_request request)
{
	string query = string() +
		"update t_filesearch_file" +
		"   set f_stamp_lost = now()" +
		" where f_stamp_seen < from_unixtime(" + utils::ultostr(f_startup) + ")" +
		"   and f_filesearch_resource = '" + utils::ultostr(request.resourceid()) + "'";
	if (mysql_query(&handle, query.c_str()))
	{
		throw e_database("Can not flush information about files ("+query+")", mysql_error(&handle));
	}
}



/***************************************************************************************************
                               FILE & SHARE REPORTING/FLUSHING ROUTINES
 ***************************************************************************************************/
t_id c_database_mysql::report_share (c_request request, string share)
{
	cerr << dec << "===SHARE " << share << " on " << utils::inet_ntoa(request.address()) << endl;
	// гарантируем наличие нужного хоста с обновленными данными об обнаружении и получем его id
	t_id resourceid;
	if (!_select_resource(resourceid, request, share)) // пытаемся найти существующий
	{
		if (!_insert_resource(resourceid, request, share)) // если не найден, пытаемся создать его
		{
			if(!_select_resource(resourceid, request, share)) // если не создался, предполагаем что он успел создаться параллельно, и пытаемся найти снова
			{
				throw e_database("Can not find or create resource record."); // финиш. не существует и не создается.
			}
		}
	}
	_update_resource(resourceid);
	return resourceid;
}

void c_database_mysql::flush_shares (c_request request)
{
	cerr << "===SHARE FLUSH on " << utils::inet_ntoa(request.address()) << endl;
	_loose_resources(request);
}



t_id c_database_mysql::report_file (c_request request, c_fileinfo fileinfo)
{
	cerr << dec << (fileinfo.container()?"---dir  ":"---file ") << fileinfo.name() << " on " << utils::inet_ntoa(request.address()) << endl;
	// гарантируем создание записи о файле или находим существующую
	t_id fileid;
	if (!_select_file(fileid, request, fileinfo)) // пытаемся найти существующий
	{
		if (!_insert_file(fileid, request, fileinfo)) // если не найден, пытаемся создать его
		{
			if(!_select_file(fileid, request, fileinfo)) // если не создался, предполагаем что он успел создаться параллельно, и пытаемся найти снова
			{
				throw e_database("Can not find or create file record."); // финиш. не существует и не создается.
			}
		}
	}
	_update_file(fileid, fileinfo);
	return fileid;
}


void c_database_mysql::flush_files (c_request request)
{
	cerr << "--- FILE FLUSH resourceid=" << request.resourceid() << endl;
	_loose_files(request);
}
