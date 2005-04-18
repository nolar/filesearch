#include "database_mysql.h"
#include <iostream>
#include <iomanip>

c_database_mysql::c_database_mysql (string host, string user, string pass, string db, unsigned int port, string socket, unsigned long flags)
	: c_database ()
{
	f_process = getpid();
	f_startup = time(NULL);
	// initializeing database engine
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



void c_database_mysql::thread_init ()
{
	if (mysql_thread_init())
	{
		throw e_database("Can not init threaded mysql variables.");
	}
}

void c_database_mysql::thread_free ()
{
	mysql_thread_end();
}

void c_database_mysql::thread_lock ()
{
}

void c_database_mysql::thread_unlock ()
{
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



c_requests c_database_mysql::fetch_requests ()
{
	c_requests result;

	string query = string() +
		"select f_protocol, f_isnetwork, f_address, f_netmask, f_port, f_share, f_username, f_password, f_workgroup, f_selfname, f_timeout" +
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
			row[10]?row[10]:"");
		result.push_back(request);
	}

	mysql_free_result(set);
	return result;
}


bool c_database_mysql::status_check (t_ipaddr address, t_proto proto, t_ipport port, string share, string username)
{
	bool result = false;

	string query = string() +
		"select count(*) from t_filesearch_status" +
		" where f_startup  = '" + escape(utils::ultostr(f_startup)) + "'" +
		"   and f_process  = '" + escape(utils::ultostr(f_process)) + "'" +
		"   and f_address  = '" + escape(utils::ultostr(address)) + "'" +
		"   and f_protocol = '" + escape(utils::ultostr(proto)) + "'" +
		"   and f_port     = '" + escape(utils::ultostr(port)) + "'" +
		"   and f_share    = '" + escape(share) + "'" +
		"   and f_username = '" + escape(username) + "'";
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

void c_database_mysql::status_renew (t_ipaddr address, t_proto proto, t_ipport port, string share, string username)
{
	string query = string() +
		"insert into t_filesearch_status (f_startup, f_process, f_address, f_protocol, f_port, f_share, f_username) values " +
		"('" + escape(utils::ultostr(f_startup)) + "','" + escape(utils::ultostr(f_process)) + "','" + escape(utils::ultostr(address))+ "','" + escape(utils::ultostr(proto))+ "','" + escape(utils::ultostr(port))+ "','" + escape(share) + "','" + escape(username) + "')";
	if (mysql_query(&handle, query.c_str()))
	{
		throw e_database("Can not insert temporary record.", mysql_error(&handle));
	}
}

void c_database_mysql::status_clean ()
{
	string query = string() +
		"delete from t_filesearch_status" +
		" where f_startup  = '" + escape(utils::ultostr(f_startup)) + "'" +
		"   and f_process  = '" + escape(utils::ultostr(f_process)) + "'";
	if (mysql_query(&handle, query.c_str()))
	{
		throw e_database("Can not clean temporary records.", mysql_error(&handle));
	}
}

//
//
//

bool c_database_mysql::_select_host (t_id &id, t_ipaddr address)
{
	bool result = false;
	string query = string() +
		"select f_filesearch_host from t_filesearch_host" +
		" where f_address = '" + escape(utils::ultostr(address)) + "'";
	if (mysql_query(&handle, query.c_str()))
	{
		throw e_database("Can not retrieve information about required host.", mysql_error(&handle));
	} else {
		MYSQL_RES * set = mysql_store_result(&handle);
		if (!set)
		{
			throw e_database("Host information retrieval failed with null set.", mysql_error(&handle));
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

bool c_database_mysql::_insert_host (t_id &id, t_ipaddr address)
{
	bool result = false;
	string query = string() +
		"insert into t_filesearch_host (f_address, f_stamp_found)" +
		" values ('" + escape(utils::ultostr(address)) + "', now())";
	if (mysql_query(&handle, query.c_str()))
	{
	} else {
		id = mysql_insert_id(&handle);
		result = true;
	}
	return result;
}

void c_database_mysql::_update_host (t_id id)
{
	string query = string() +
		"update t_filesearch_host" +
		"   set f_stamp_seen = now()" +
		" where f_filesearch_host = '" + escape(utils::ultostr(id)) + "'";
	if (mysql_query(&handle, query.c_str()))
	{
		throw e_database("Can not update information about host.", mysql_error(&handle));
	}
}

//
//
//
bool c_database_mysql::_select_item (t_id &id, t_id host, t_proto proto, t_ipport port, string username, string password, string hostname, string netbiosshare, string netbiosname, string netbiosgroup)
{
	bool result = false;
	string query = string() +
		"select f_filesearch_item from t_filesearch_item" +
		" where f_filesearch_host = '" + escape(utils::ultostr(host)) + "'" +
		"   and f_proto           = '" + escape(utils::ultostr(proto)) + "'" +
		"   and f_netbiosshare    = '" + escape(netbiosshare) + "'" +
		"   and f_username        = '" + escape(username) + "'";
	if (mysql_query(&handle, query.c_str()))
	{
		throw e_database("Can not retrieve information about requested item.", mysql_error(&handle));
	} else {
		MYSQL_RES * set = mysql_store_result(&handle);
		if (!set)
		{
			throw e_database("Item information retrieval failed with null set.", mysql_error(&handle));
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

bool c_database_mysql:: _insert_item (t_id &id, t_id host, t_proto proto, t_ipport port, string username, string password, string hostname, string netbiosshare, string netbiosname, string netbiosgroup)
{
	bool result = false;
	string query = string() +
		"insert into t_filesearch_item (f_filesearch_host, f_proto, f_port, f_username, f_password, f_hostname, f_netbiosshare, f_netbiosname, f_netbiosgroup, f_stamp_found)" +
		" values ('" + escape(utils::ultostr(host)) + "','" + escape(utils::ultostr(proto)) + "','" + escape(utils::ultostr(port)) + "','" +
			escape(username) + "','" + escape(password) + "','" + escape(hostname) + "','" +
			escape(netbiosshare) + "','" + escape(netbiosname) + "','" + escape(netbiosgroup) + "', now())";
	if (mysql_query(&handle, query.c_str()))
	{
	} else {
		id = mysql_insert_id(&handle);
		result = true;
	}
	return result;
}

void c_database_mysql::_update_item (t_id id)
{
	string query = string() +
		"update t_filesearch_item" +
		"   set f_stamp_seen = now()" +
		" where f_filesearch_item = '" + escape(utils::ultostr(id)) + "'";
	if (mysql_query(&handle, query.c_str()))
	{
		throw e_database("Can not update information about item.", mysql_error(&handle));
	}
}

//
//
//
bool c_database_mysql::_select_file (t_id &id, t_id item, string path, string name)
{
	bool result = false;
	string query = string() +
		"select f_filesearch_file from t_filesearch_file" +
		" where f_filesearch_item = '" + escape(utils::ultostr(item)) + "'" +
		"   and f_path           = '" + escape(path) + "'" +
		"   and f_name           = '" + escape(name) + "'";
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

bool c_database_mysql:: _insert_file (t_id &id, t_id item, string path, string name)
{
	bool result = false;
	string query = string() +
		"insert into t_filesearch_file (f_filesearch_item, f_path, f_name, f_stamp_found)" +
		" values ('" + escape(utils::ultostr(item)) + "','" + escape(path) + "','" + escape(name) + "', now())";
	if (mysql_query(&handle, query.c_str()))
	{
	} else {
		id = mysql_insert_id(&handle);
		result = true;
	}
	return result;
}

void c_database_mysql::_update_file (t_id id, size_t size, time_t time)
{
	string query = string() +
		"update t_filesearch_file" +
		"   set f_stamp_seen = now()" +
		"     , f_size       = '" + escape(utils::ultostr(size)) + "'" +
		"     , f_time       = '" + escape(utils::ultostr(time)) + "'" +
		" where f_filesearch_file = '" + escape(utils::ultostr(id)) + "'";
	if (mysql_query(&handle, query.c_str()))
	{
		throw e_database("Can not update information about file.", mysql_error(&handle));
	}
}



//
//
//
t_id c_database_mysql::report_item (c_request request, string share)
{
	cerr << dec << "===SHARE " << share << " on " << utils::inet_ntoa(request.address()) << endl; return 0;
/*	// гарантируем наличие нужного хоста с обновленными данными об обнаружении и получем его id
	t_id hostid;
	if (!_select_host(hostid, address)) // пытаемся найти существующий
	{
		if (!_insert_host(hostid, address)) // если не найден, пытаемся создать его
		{
			if(!_select_host(hostid, address)) // если не создался, предполагаем что он успел создаться параллельно, и пытаемся найти снова
			{
				throw e_database("Can not find or create host record."); // финиш. не существует и не создается.
			}
		}
	}
	_update_host(hostid);
	// гарантируем наличие нужного итема с обновленными данными об обнаружении и получаем его id
	t_id itemid;
	if (!_select_item(itemid, hostid, proto, port, username, password, hostname, netbiosshare, netbiosname, netbiosgroup)) // пытаемся найти существующий
	{
		if (!_insert_item(itemid, hostid, proto, port, username, password, hostname, netbiosshare, netbiosname, netbiosgroup)) // если не найден, пытаемся создать его
		{
			if(!_select_item(itemid, hostid, proto, port, username, password, hostname, netbiosshare, netbiosname, netbiosgroup)) // если не создался, предполагаем что он успел создаться параллельно, и пытаемся найти снова
			{
				throw e_database("Can not find or create item record."); // финиш. не существует и не создается.
			}
		}
	}
	_update_item(itemid);
	// имеем желанный itemid для добавления файлов, возвращаем его
	return itemid;*/
}

/*extern iconv_t c_engine_samba_iconv;
t_id c_database_mysql::found_file (t_id item, c_path path, c_stat stat)
{
	cerr << dec << "FILE " << "/";
	for (c_path::iterator i = path.begin(); i!=path.end(); i++)
		cerr << utils::iconv(c_engine_samba_iconv, *i) << "/";
	cerr << "                                   (size " << stat.bytes() << ", ctime " << stat.ctime() << ", mtime " << stat.mtime() << ", container " << (stat.container()?"YES":"NO") << ")" << endl;
	return 0;
}*/

/*void c_database_mysql::found_file (t_id item, string path, c_entries entries)
{
	c_entries::iterator i;
	for (i = entries.begin(); i != entries.end(); i++)
	{
		// гарантируем создание записи о файле или находим существующую
		t_id fileid;
		if (!_select_file(fileid, item, path, i->name())) // пытаемся найти существующий
		{
			if (!_insert_file(fileid, item, path, i->name())) // если не найден, пытаемся создать его
			{
				if(!_select_file(fileid, item, path, i->name())) // если не создался, предполагаем что он успел создаться параллельно, и пытаемся найти снова
				{
					throw e_database("Can not find or create file record."); // финиш. не существует и не создается.
				}
			}
		}
		_update_file(fileid, i->size(), i->time());
	}
}*/
