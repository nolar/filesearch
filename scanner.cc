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
#include "module.h"

c_database * database = NULL;
c_forker * forker = NULL;

void module_init ()
{
	database->thread_init();
	utils::signal_uncatch(SIGCHLD);
	utils::signal_unblock(SIGCHLD);
	utils::signal_ignore(SIGPIPE);
}

void module_free ()
{
	database->thread_free();
}


inline void scan_address (t_ipaddr address, c_request request)
{
	// проверяем, не была ли такая шара с такого адреса уже найдена
	bool already = database->status_check(address, request.proto(), request.port(), request.share(), request.username());
	// если таковой еще нет, то сканируем эту шару на этом компьютере
	if (!already)
	{
		// сканируем компьютер или конкретную шару
		switch (request.proto())
		{
			case proto_smb:
				module_smb__request = request;
				module_smb__request = address;
				forker->fork(module_smb, module_init, module_free);
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
		// connecting to database
		database = new c_database_mysql("", "filesearch", "filesearch", "filesearch", 0, "", 0);
		database->status_clean();
		// retrieving options from database config table
		// creating fork manager
		forker = new c_forker(5); //!!! max_children should be option
		// retrieving list of address requests
		c_requests requests = database->fetch_requests();
		c_requests::iterator request;
		for (request = requests.begin(); request != requests.end(); request++)
		{
			//!!! debug output. delete it later.
//			cerr << "CHECKING: ";
			cerr << "Address=" << dec << setw(10) << request->address() << "(" << hex << setw(8) << request->address() << ")";
			cerr << " ";
			cerr << "Netmask=" << dec << setw( 2) << request->netmask();
			cerr << " => ";
			cerr << dec << setw(10) << request->address_from() << "(" << hex << setw(8) << request->address_from() << ")";
			cerr << "-";
			cerr << dec << setw(10) << request->address_till() << "(" << hex << setw(8) << request->address_till() << ")";
//			cerr << " // Share=" << request->share();
//			cerr << " // Accrd=" << request->username() << ":" << request->password();
			cerr << endl;

			// пробежка по всем адресам проверяемого блока
			t_ipaddr address_from = request->address_from();
			t_ipaddr address_till = request->address_till();
			for (t_ipaddr address = address_from; address < address_till; address++)
				{scan_address(address     , *request);}
			if (address_from <= address_till)
				{scan_address(address_till, *request);}
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
