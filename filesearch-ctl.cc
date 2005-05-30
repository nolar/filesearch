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
#include "globals.h"
#include "c_stream.h"
#include "e_basic.h"
#include "e_database.h"
#include "database_mysql.h"
#include "forker.h"
#include "thread.h"
#include "thread_smb.h"

#include "c_signed.h"
#include "c_unsigned.h"
#include "c_string.h"
#include "c_path.h"
#include "c_ipaddr.h"
#include "c_stamp.h"

c_database * database = NULL;
c_forker * forker = NULL;

inline void scan_address (c_request request)
{
	DEBUG("Checking status of protocol='"+request.protocol().ascii()+"',ip='"+request.address.ascii()+"',port='"+request.port().ascii()+"',share='"+request.share()+"',username='"+request.username()+"'.");
	// проверяем, не была ли такая шара с такого адреса уже найдена
	bool already = database->status_check(request);
	// если таковой еще нет, то сканируем эту шару на этом компьютере
	if (!already)
	{
		// сканируем компьютер или конкретную шару
		DEBUG("Creating scanner for protocol='"+request.protocol().ascii()+"',ip='"+request.address.ascii()+"',port='"+request.port().ascii()+"',share='"+request.share()+"',username='"+request.username()+"'.");
		switch (request.protocol())
		{
			case protocol_smb:
				thread_smb__request = request;
				forker->fork(thread_smb, thread_init, thread_free, thread_catch);
				break;
			case protocol_ftp:
				//todo
				break;
			case protocol_http:
				//todo
				break;
			default:
				break;
		}
	} else {
		DEBUG("Skipping scanning of protocol='"+request.protocol().ascii()+"',ip='"+request.address.ascii()+"',port='"+request.port().ascii()+"',share='"+request.share()+"',username='"+request.username()+"'.");
	}
}



int main (int argc, char ** argv, char ** env) {
	int exitcode = 0;
	try
	{
		DEBUG("Main filesearcher started.");
		// получение параметров вызова программы и занесение их в переменные
		//!!!
		s_log.fd(1);
		s_debug.fd(2);
//!!!		s_null(open(io::fd_null_path, O_RDWR)); //!!!
		// connecting main thread to database
		DEBUG("Connecting to database.");
		database = new c_database_mysql("", "filesearch", "filesearch", "filesearch", 0, "", 0);
		database->status_init();
		database->status_clean();
		DEBUG("Connection to database established.");
		// retrieving options from database config table
		//!!!
		// creating fork manager
		forker = new c_forker(default_children_count); //!!! max_children should be configurable option
		// retrieving list of address requests
		DEBUG("Fetching requests from database.");
		c_requests requests = database->fetch_requests();
//!!!		DEBUG("Fetched "+convert::ul2str(requests.size())+" requests.");
		for (c_requests::const_iterator request = requests.begin(); request != requests.end(); request++)
		{
			// пробежка по всем адресам проверяемого блока
			c_ipaddr address_from = request->address().first(); 
			c_ipaddr address_till = request->address().last (); 
			if (request.isnetwork()) { address_from++; address_till--; }
			for (c_ipaddr address = address_from; address <= address_till; address++)
				{c_request concrete = *request; concrete.address(address); scan_address(concrete);}
		}
		// freeing engine's resources
	}
	catch (std::exception &e)
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
//!!!	DEBUG("Main filesearcher exited: "+convert::pstatus2print(exitcode)+".");
	return exitcode;
}
