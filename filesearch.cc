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
#include "fdstream"
#include "forker.h"
#include "thread.h"
#include "thread_smb.h"

c_database * database = NULL;
c_forker * forker = NULL;

inline void scan_address (c_request request)
{
	// проверяем, не была ли такая шара с такого адреса уже найдена
	bool already = database->status_check(request);
	// если таковой еще нет, то сканируем эту шару на этом компьютере
	if (!already)
	{
		// сканируем компьютер или конкретную шару
		switch (request.proto())
		{
			case proto_smb:
				thread_smb__request = request;
				forker->fork(thread_smb, thread_init, thread_free);
				break;
			case proto_ftp:
				break;
			case proto_http:
				break;
			default:
			case proto_unknown:
				break;
		}
	}
}



int main (int argc, char ** argv, char ** env) {
	int exitcode = 0;
	try
	{
		// получение параметров вызова программы и занесение их в переменные
		//!!!
		// connecting main thread to database
		database = new c_database_mysql(getpid(), time(NULL), "", "filesearch", "filesearch", "filesearch", 0, "", 0);
		database->status_clean();
		// retrieving options from database config table
		//!!!
		// creating fork manager
		forker = new c_forker(5); //!!! max_children should be configurable option
		// retrieving list of address requests
		c_requests requests = database->fetch_requests();
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
		cerr << "Address convertion error: " <<  e.what() << endl;
		exitcode = -4;
	}
	catch (e_database &e)
	{
		cerr << "Database error: " <<  e.what() << endl;
		exitcode = -3;
	}
	catch (exception &e)
	{
		cerr << "Unknown exception: " << e.what() << endl;
		exitcode = -2;
	}
	catch (...)
	{
		cerr << "Unknown exception without message" << endl;
		exitcode = -1;
	}
	// waiting for all children to stop
	while (!forker->empty())
	{
		utils::signal_pause();
	}
	// freeing database
	if (database)
	{
		try { database->status_clean(); }
		catch (e_database) {}
		delete database; database = NULL;
	}
	return exitcode;
}
