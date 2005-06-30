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
#include <string>
#include <vector>
#include <stack>
#include <exception>
#include <sys/time.h>
#include <errno.h>
#include <iostream>
#include <iomanip>

#include "config.h"
#include "globals.h"
#include "c_stream.h"
#include "e_basic.h"
#include "e_samba.h"

#include "c_signed.h"
#include "c_unsigned.h"
#include "c_string.h"
#include "c_path.h"
#include "c_ipaddr.h"

std::string address   ;
std::string rootpath  ;
std::string username  ;
std::string password  ;
std::string workgroup ;
std::string selfname  ;
unsigned    timeout   ;
unsigned    depth     ;

int main(int argc, char ** argv, char ** env) {
	int exitcode = 0;
	try
	{
		s_log.fd(1);
		s_debug.fd(2);
		c_stream s_task(3);
		// получение параметров вызова программы и занесение их в переменные
		std::cout << "Address  : "; std::cin >> address;
		std::cout << "Rootpath : "; std::cin >> rootpath ; if (rootpath == "-") rootpath = "";
		std::cout << "Username : "; std::cin >> username ; if (username == "-") username = "";
		std::cout << "Password : "; std::cin >> password ; if (password == "-") password = "";
		std::cout << "Workgroup: "; std::cin >> workgroup; if (workgroup== "-") workgroup= "";
		std::cout << "Selfname : "; std::cin >> selfname ; if (selfname == "-") selfname = "";
		std::cout << "Timeout  : "; std::cin >> timeout;
		std::cout << "Depth    : "; std::cin >> depth;
		//
		timeval timer;
		timer.tv_sec  = default_timeout_task__sec;
		timer.tv_usec = default_timeout_task__usec;
		c_stream::t_map task;
		std::cerr<< "Creating..." << std::endl;
		task[ipc_task_ipaddr   ] = new c_ipaddr  (address  );
		task[ipc_task_rootpath ] = new c_path    (rootpath );
		task[ipc_task_username ] = new c_string  (username );
		task[ipc_task_password ] = new c_string  (password );
		task[ipc_task_workgroup] = new c_string  (workgroup);
		task[ipc_task_selfname ] = new c_string  (selfname );
		task[ipc_task_timeout  ] = new c_unsigned(timeout  );
		task[ipc_task_depth    ] = new c_unsigned(depth    );
		std::cerr<< "Writing..." << std::endl;
		s_task.write_map(task, &timer);
		std::cerr<< "Flushing.." << std::endl;
		close(3);
//!!!		DEBUG("Reading of parameters finished with status "+io::sstatus2print(status)+". Starting to parse them.");
	}
	catch (e_basic &e)
	{
		LOG("Exception: "+e.what());
		exitcode = 1;
	}
	catch (std::exception &e)
	{
		LOG("Exception: "+e.what());
		exitcode = 1;
	}
	catch (...)
	{
		LOG("Exception with no type and message.");
		exitcode = 1;
	}
	return exitcode;
}
