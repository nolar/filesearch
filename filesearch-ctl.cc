/*
 * Сканер ресурсов обслуживаемых компьютеров.
 *
 * Формат входных данных (stdin):
 *
 * Формат выходных данных (stdout):
 *
 * (a) Sergei Vasilyev aka nolar 2005
 */

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include "config.h"
#include "typedefs.h"
#include "e_database.h"
#include "database_mysql.h"
#include "forker.h"
#include "thread.h"
#include "thread_smb.h"
#include "options.h"
#include "io.h"
#include "convert.h"

c_database * database = NULL;
c_forker * forker = NULL;

inline void scan_address (c_request request)
{
	DEBUG("Checking status of ip='"+convert::ipaddr2print(request.address())+"',share='"+request.share()+"',username='"+request.username()+"'.");
	// проверяем, не была ли такая шара с такого адреса уже найдена
	bool already = database->status_check(request);
	// если таковой еще нет, то сканируем эту шару на этом компьютере
	if (!already)
	{
		// сканируем компьютер или конкретную шару
		switch (request.proto())
		{
			case proto_smb:
				DEBUG("Creating (smb) scanner for ip='"+convert::ipaddr2print(request.address())+"',share='"+request.share()+"',username='"+request.username()+"'.");
				thread_smb__request = request;
				forker->fork(thread_smb, thread_init, thread_free, thread_catch);
				break;
			case proto_ftp:
				DEBUG("Creating (ftp) scanner for ip='"+convert::ipaddr2print(request.address())+"',share='"+request.share()+"',username='"+request.username()+"'.");
				break;
			case proto_http:
				DEBUG("Creating (http) scanner for ip='"+convert::ipaddr2print(request.address())+"',share='"+request.share()+"',username='"+request.username()+"'.");
				break;
			default:
			case proto_unknown:
				DEBUG("Unknown protocol in request for ip='"+convert::ipaddr2print(request.address())+"',share='"+request.share()+"',username='"+request.username()+"'.");
				break;
		}
	} else {
		DEBUG("Request for ip='"+convert::ipaddr2print(request.address())+"',share='"+request.share()+"',username='"+request.username()+"' already scanned. Skipping.");
	}
}



int main (int argc, char ** argv, char ** env) {
	int exitcode = 0;
	try
	{
		DEBUG("Main filesearcher started.");
		// получение параметров вызова программы и занесение их в переменные
		//!!!
		io::fd_null = open(io::fd_null_path, O_RDWR);
		// connecting main thread to database
		DEBUG("Connecting to database.");
		database = new c_database_mysql("", "filesearch", "filesearch", "filesearch", 0, "", 0);
		database->status_init();
		database->status_clean();
		DEBUG("Connection to database established.");
		// retrieving options from database config table
		//!!!
		// creating fork manager
		forker = new c_forker(options::default_children); //!!! max_children should be configurable option
		// retrieving list of address requests
		DEBUG("Fetching requests from database.");
		c_requests requests = database->fetch_requests();
		DEBUG("Fetched "+convert::ul2str(requests.size())+" requests.");
		c_requests::iterator request;
		for (request = requests.begin(); request != requests.end(); request++)
		{
			// пробежка по всем адресам проверяемого блока
			t_ipaddr address_from = request->address_from();
			t_ipaddr address_till = request->address_till();
			for (t_ipaddr address = address_from; address < address_till; address++)
				{c_request concrete = *request; concrete.address(address     ); scan_address(concrete);}
			if (address_from <= address_till)
				{c_request concrete = *request; concrete.address(address_till); scan_address(concrete);}
		}
		// freeing engines resources
	}
	catch (exception &e)
	{
		LOG("Exception: "+e.what());
		exitcode = 1;
	}
	catch (e_basic &e)
	{
		LOG("Exception: "+e.what());
		exitcode = 1;
	}
	catch (...)
	{
		LOG("Exception without type and message.");
		exitcode = 1;
	}
	// waiting for all children to stop
	DEBUG("Waiting for children.");
	while (!forker->empty())
	{
		c_forker::signal_pause();
	}
	DEBUG("All children exited.");
	// freeing database
	if (database)
	{
		DEBUG("Closing database connection.");
		try { database->status_clean(); }
		catch (e_database) {}
		delete database; database = NULL;
		DEBUG("Closed database connection.");
	}
	DEBUG("Main filesearcher exited: "+convert::pstatus2print(exitcode)+".");
	return exitcode;
}
