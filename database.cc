#include "database.h"

c_database::c_database (t_pident a_process, t_time a_startup)
{
	f_process = a_process;
	f_startup = a_startup;
}

c_database::~c_database ()
{
}
