/*
 * Сканер ресурсов обслуживаемых компьютеров.
 *
 * Формат входных данных (stdin):
 *
 * Формат выходных данных (stdout):
 *
 * (a) Sergei Vasilyev aka nolar 2005
 */

#include "e_database.h"
#include "database_mysql.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#include "fdstream"
#include "forker.h"
#include "thread.h"
#include "thread_smb.h"
#include "options.h"

c_database * database = NULL;
c_forker * forker = NULL;

inline void scan_address (c_request request)
{
	DEBUG("Checking status of ip='"+utils::inet_ntoa(request.address())+"',share='"+request.share()+"',username='"+request.username()+"'.");
	// проверяем, не была ли такая шара с такого адреса уже найдена
	bool already = database->status_check(request);
	// если таковой еще нет, то сканируем эту шару на этом компьютере
	if (!already)
	{
		// сканируем компьютер или конкретную шару
		switch (request.proto())
		{
			case proto_smb:
				DEBUG("Creating (smb) scanner for ip='"+utils::inet_ntoa(request.address())+"',share='"+request.share()+"',username='"+request.username()+"'.");
				thread_smb__request = request;
				forker->fork(thread_smb, thread_init, thread_free);
				break;
			case proto_ftp:
				DEBUG("Creating (ftp) scanner for ip='"+utils::inet_ntoa(request.address())+"',share='"+request.share()+"',username='"+request.username()+"'.");
				break;
			case proto_http:
				DEBUG("Creating (http) scanner for ip='"+utils::inet_ntoa(request.address())+"',share='"+request.share()+"',username='"+request.username()+"'.");
				break;
			default:
			case proto_unknown:
				DEBUG("Unknown protocol in request for ip='"+utils::inet_ntoa(request.address())+"',share='"+request.share()+"',username='"+request.username()+"'.");
				break;
		}
	} else {
		DEBUG("Request for ip='"+utils::inet_ntoa(request.address())+"',share='"+request.share()+"',username='"+request.username()+"' already scanned. Skipping.");
	}
}



int main (int argc, char ** argv, char ** env) {
	int exitcode = 0;
	try
	{
		DEBUG("Main filesearcher started.");
		// получение параметров вызова программы и занесение их в переменные
		//!!!
		options::fd_null = open("/dev/null", O_WRONLY);
		// connecting main thread to database
		DEBUG("Connecting to database.");
		database = new c_database_mysql(getpid(), time(NULL), "", "filesearch", "filesearch", "filesearch", 0, "", 0);
		database->status_clean();
		DEBUG("Connection to database established.");
		// retrieving options from database config table
		//!!!
		// creating fork manager
		forker = new c_forker(5); //!!! max_children should be configurable option
		// retrieving list of address requests
		DEBUG("Fetching requests from database.");
		c_requests requests = database->fetch_requests();
		DEBUG("Fetched "+utils::ultostr(requests.size())+" requests.");
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
	catch (e_address &e)
	{
		LOG("Exception in address convertation: "+e.what());
		exitcode = 4;
	}
	catch (e_database &e)
	{
		LOG("Exception in database: "+e.what());
		exitcode = 3;
	}
	catch (exception &e)
	{
		LOG("Exception: "+e.what());
		exitcode = 2;
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
		utils::signal_pause();
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
	DEBUG("Main filesearcher exited with code "+utils::ultostr(exitcode)+".");
	return exitcode;
}
