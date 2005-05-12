#include <unistd.h>
#include "database.h"

c_database::c_database ()
{
}

c_database::~c_database ()
{
}

void c_database::status_init ()
{
	f_process = getpid();
	f_startup = fetch_startup();
}

void c_database::status_init (t_pident a_process, t_time a_startup)
{
	f_process = a_process;
	f_startup = a_startup;
}
