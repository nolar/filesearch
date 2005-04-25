#include "database.h"

c_database::c_database (pid_t a_process, time_t a_startup)
{
	f_process = a_process;
	f_startup = a_startup;
}

c_database::~c_database ()
{
}
