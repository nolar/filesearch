#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <map>
#include <vector>
#include <string>
#include <errno.h>

#include "thread.h"
#include "database.h"
#include "forker.h"
#include "c_stream.h"
#include "e_stream.h"
#include "e_stream_eof.h"
#include "e_stream_timeout.h"

#include "c_object.h"
#include "c_stopsign.h"
#include "c_identifier.h"
#include "c_signed.h"
#include "c_unsigned.h"
#include "c_double.h"
#include "c_flag.h"
#include "c_stamp.h"
#include "c_string.h"
#include "c_ipaddr.h"
#include "c_path.h"
#include "c_event.h"
#include "c_protocol.h"


extern c_database * database;
extern c_forker * forker;

void thread_init ()
{
	database = database->duplicate(); //!!! exscentric way to make copy of existing database. better to replace it with create_database(), which will use connection parameters to determine type of database.
	forker = NULL;
	c_forker::signal_uncatch(SIGCHLD);
	c_forker::signal_unblock(SIGCHLD);
	c_forker::signal_ignore(SIGPIPE);
}

void thread_free ()
{
	delete database;
}

int thread_catch (std::exception * se, e_basic * be)
{
//!!!	kill(childpid, SIGKILL);
//!!!	wait for child? or it can be left orphaned, when controlller process is killed.
	delete database;
	return 1;
}

bool thread_wrap (std::string command, c_request request,
	t_wrap_action_resource action_resource,
	t_wrap_action_dir      action_dir     ,
	t_wrap_action_file     action_file    ,
	t_wrap_action_dataflow action_dataflow)
{
	pid_t pid; std::string pidstr;
	std::string urlstr = request.ascii();

	// creating fd for stdin & stdout of scan process
	DEBUG("Creating pipes to scanner process for url '"+urlstr+"'.");
	int ifd[2]; if (pipe(ifd) == -1) {                               throw e_basic(__FILE__,__LINE__,"Can not create in pipe.", errno); } //e_basic???!!!
	DEBUG("Created pipe "+c_unsigned(ifd[0]).ascii()+"<->"+c_unsigned(ifd[1]).ascii()+" for task stream of scanner process for url '"+urlstr+"'.");
	int ofd[2]; if (pipe(ofd) == -1) { close(ifd[0]); close(ifd[1]); throw e_basic(__FILE__,__LINE__,"Can not create out pipe.", errno); } //e_basic???!!!
	DEBUG("Created pipe "+c_unsigned(ofd[0]).ascii()+"<->"+c_unsigned(ofd[1]).ascii()+" for data stream of scanner process for url '"+urlstr+"'.");

	try
	{
		// executing scan process with substituted fds
		STATUS("starting '"+urlstr+"'");
		std::map<int,int> fds;
		std::vector<std::string> arg;
		std::vector<std::string> env;
		fds[fd_ipc_task ] = ifd[0];//????????!!!!!!!
		fds[fd_ipc_data ] = ofd[1];//????????!!!!!!!!
//???		fds[fd_ipc_log  ] = fd_null;
//???		fds[fd_ipc_debug] = fd_null;
		arg.push_back(request.ipaddr().ascii());
		arg.push_back(request.share().ascii());
		pid = c_forker::exec(command, arg, env, fds);
		pidstr = std::string(c_unsigned(pid).ascii());
	}
	catch (e_basic & e)
	{
		close(ifd[0]); close(ofd[1]);
		close(ifd[1]); close(ofd[0]);
		STATUS("");
		throw;
	}
	DEBUG("Created scanner process "+pidstr+" for url '"+urlstr+"'.");

	// creating streams for opened scanner process
	close(ifd[0]); close(ofd[1]);
	c_stream s_task(ifd[1]); s_task.set_min_timeout(default_timeout_task__sec, default_timeout_task__usec);
	c_stream s_data(ofd[0]);

	try
	{ 
		// sending parameters to scanner process
		c_stream::t_map task;
		task[ipc_task_ipaddr   ] = new c_ipaddr  (request.ipaddr   ());
		task[ipc_task_rootpath ] = new c_path    (request.share    ().ascii(), default_path_delimiter);
		task[ipc_task_username ] = new c_string  (request.username ());
		task[ipc_task_password ] = new c_string  (request.password ());
		task[ipc_task_workgroup] = new c_string  (request.workgroup());
		task[ipc_task_selfname ] = new c_string  (request.selfname ());
		task[ipc_task_timeout  ] = new c_unsigned(request.timeout  ());
		task[ipc_task_depth    ] = new c_unsigned(request.depth    ());
		DEBUG("Writing task for scanner process "+pidstr+".");
		s_task.write_map(task);
	}
	catch (e_basic & e) {
		close(ifd[1]); close(ofd[0]);
		STATUS("");
		throw;
	}

	// reading and handling output of scanner process
	timeval timer;
	bool datacomplete = false;
	try
	{
		STATUS("controlling "+urlstr);
		do
		{
			DEBUG("Getting reply from scanner process "+pidstr+".");
			timer.tv_sec = request.timeout().get(); timer.tv_usec = 0;
			c_stream::t_map data = s_data.read_map(&timer);
			c_event event; if (data.find(ipc_data_event) != data.end()) { event = data[ipc_data_event].as_event(); }
			DEBUG("Got event='"+event.ascii()+"' from scanner process "+pidstr+".");
			//
			if (action_dataflow && !datacomplete)
			{
				action_dataflow(request);
				datacomplete = true;
			}
			//
			if (action_resource && event.is_resource())
			{
				c_path  path  = data[ipc_data_path].as_path();
				action_resource(request, path);
			} else
			if (action_dir      && event.is_dir())
			{
				c_path  path  = data[ipc_data_path ].as_path ();
				c_stamp ctime = data[ipc_data_ctime].as_stamp();
				c_stamp mtime = data[ipc_data_mtime].as_stamp();
				c_fileinfo fileinfo(path, true, 0, ctime, mtime);
				action_dir(request, fileinfo);
			} else
			if (action_file     && event.is_file())
			{
				c_path     path  = data[ipc_data_path ].as_path();
				c_unsigned size  = data[ipc_data_size ].as_unsigned();
				c_stamp    ctime = data[ipc_data_ctime].as_stamp();
				c_stamp    mtime = data[ipc_data_mtime].as_stamp();
				c_fileinfo fileinfo(path, false, size, ctime, mtime);
				action_file(request, fileinfo);
			} else
			{/* unkown event or known event with no handler - this is not error - just ignore*/}
		} while (true);
	}
	catch (e_stream_timeout & e) {}
	catch (e_stream_eof & e) {}
	catch (e_basic & e) {
		close(ifd[1]); close(ofd[0]);
		STATUS("");
		throw;
	}
	close(ifd[1]); close(ofd[0]);
	DEBUG("Reading of data from scanner process "+pidstr+" finished.");

	// closing process
	STATUS("closing "+urlstr);
	DEBUG("Waiting for scanner process "+pidstr+" to finish.");
	int wstat;
	DEBUG("Sending signal to process "+pidstr+".");
	kill(pid, SIGKILL);
	DEBUG("Sent signal to process "+pidstr+": "+(errno?"ok":strerror(errno))+".");
	int counter = 0;
	while (c_forker::wait_child_hang(&wstat) > 0)
	{
		datacomplete = datacomplete && WIFEXITED(wstat) && !WEXITSTATUS(wstat);
		counter++;
		DEBUG("Scanner process "+pidstr+" exited: "+c_forker::wstat(wstat)+".");
	}
	DEBUG("KILLING FINISHED lastcode="+strerror(errno)+".");
	STATUS("");
	return datacomplete;
}
