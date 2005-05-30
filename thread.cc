#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <map>
#include <vector>
#include <string>

#include <iostream>
#include <iomanip>

#include "thread.h"
#include "database.h"
#include "forker.h"
#include "c_stream.h"

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
	t_wrap_action_dir      action_dir,
	t_wrap_action_file     action_file,
	t_wrap_action_enter    action_enter,
	t_wrap_action_leave    action_leave,
	t_wrap_action_start    action_start)
{
	// creating fd for stdin & stdout of scan process
//!!!	DEBUG("Creating pipes to scanner process for ip='"+convert::ipaddr2print(request.address())+"',share='"+request.share()+"',username='"+request.username()+"'.");
	int ifd[2]; if (pipe(ifd) == -1) throw e_basic("can not create in pipe!!!", errno, strerror(errno));//e_basic???!!!
//!!!	DEBUG("Created pipe "+convert::fd2print(ifd[0])+"<->"+convert::fd2print(ifd[1])+" for task stream of scanner process for ip='"+convert::ipaddr2print(request.address())+"',share='"+request.share()+"',username='"+request.username()+"'.");
	int ofd[2]; if (pipe(ofd) == -1) throw e_basic("can not create out pipe!!!", errno, strerror(errno));//e_basic???!!!
//!!!	DEBUG("Created pipe "+convert::fd2print(ofd[0])+"<->"+convert::fd2print(ofd[1])+" for data stream of scanner process for ip='"+convert::ipaddr2print(request.address())+"',share='"+request.share()+"',username='"+request.username()+"'.");
	c_stream s_task();

	//
	std::string url = request.protocol().ascii() + "://" + request.address().ascii() + "/" + request.share();
	// executing scan process with substituted fds
	STATUS("starting "+url);
	std::map<int,int> fds;
	std::vector<std::string> arg;
	std::vector<std::string> env;
	fds[io::fd_task] = ifd[0];//????????!!!!!!!
	fds[io::fd_data] = ofd[1];//????????!!!!!!!!
//???	fds[io::fd_debug] = io::fd_null;
	arg.push_back(request.address().ascii());
	arg.push_back(request.share());
	t_pident pid = c_forker::exec(command, arg, env, fds);
//!!!	DEBUG("Created scanner process "+c_unsigned(pid).ascii()+" for ip='"+convert::ipaddr2print(request.address())+"',share='"+request.share()+"',username='"+request.username()+"'.");
	close(ifd[0]); close(ofd[1]);

	//
	c_stream s_task(ifd[1]); s_task.set_min_timer(default_timeout_task__sec, default_timeout_task__usec);
	c_stream s_data(ofd[0]);

	// sending parameters to scanner process
	c_stream::t_map task;
	task[ipc_task_ipaddr   ] = new c_address (request.address  ());
	task[ipc_task_rootpath ] = new c_path    (request.share    ());
	task[ipc_task_username ] = new c_string  (request.username ());
	task[ipc_task_password ] = new c_string  (request.password ());
	task[ipc_task_workgroup] = new c_string  (request.workgroup());
	task[ipc_task_selfname ] = new c_string  (request.selfname ());
	task[ipc_task_timeout  ] = new c_unsigned(request.timeout  ());
	task[ipc_task_depth    ] = new c_unsigned(request.depth    ());
	DEBUG("Writing task for scanner process "+c_unsigned(pid).ascii()+".");
	s_task.write_map(task);
	for (c_stream::t_map::iterator i = task.begin(); i != task.end(); i++) delete i->second;
	if (s_task.status()) throw e_basic("Can not write task to scanner process "+c_unsigned(pid)+" status "+s_task.status_text()+".");

	// reading and handling output of scanner process
	STATUS("controlling "+url);
	timeval timer;
	bool datacomplete = false;
	do {
		DEBUG("Getting reply from scanner process "+c_unsigned(pid).ascii()+".");
		timer.tv_sec = request.timeout(); timer.tv_usec = 0;
		c_stream::t_map data = s_data.read_map(&timer);
		c_action action; if (data.find(ipc_data_action) != data.end()) { action = data[ipc_data_action].as_action(); }
		if (s_data.status()) DEBUG("Can not read data from scanner process "+c_unsigned(pid).ascii()+" status "+s_data.status_text()+".");
		else                 DEBUG("Got action='"+action->ascii()+"' and "+convert::ui2str(data.size())+" data lines from scanner process "+c_unsigned(pid).ascii()+".");
		if (!status && !action.empty())
		{
			//
			if (action_start && !datacomplete)
			{
				action_start(request);
				datacomplete = true;
			}
			//
			if (action_resource && action == options::action_code_for_resource && data.size() >= 1)
			{
				t_path path  = convert::str2path(data[0]);
				action_resource(request, path);
			} else
			if (action_dir      && action == options::action_code_for_dir      && data.size() >= 3)
			{
				t_path path  = convert::str2path(data[0]);
				t_time ctime = convert::str2time(data[1]);
				t_time mtime = convert::str2time(data[2]);
				c_fileinfo fileinfo(path, true, 0, ctime, mtime);
				action_dir(request, fileinfo);
			} else
			if (action_file     && action == options::action_code_for_file     && data.size() >= 4)
			{
				t_path path  = convert::str2path(data[0]);
				t_size size  = convert::str2size(data[1]);
				t_time ctime = convert::str2time(data[2]);
				t_time mtime = convert::str2time(data[3]);
				c_fileinfo fileinfo(path, false, size, ctime, mtime);
				action_file(request, fileinfo);
			} else
			if (action_enter    && action == options::action_code_for_enter    && data.size() >= 1)
			{
				t_path path = convert::str2path(data[0]);
				action_enter(request, path);
			} else
			if (action_leave    && action == options::action_code_for_leave    && data.size() >= 1)
			{
				t_path path = convert::str2path(data[0]);
				action_leave(request, path);
			} else
			{/* unkown action or known action with no handler */}
		}
	} while (!status);
	close(ifd[1]); close(ofd[0]);
	DEBUG("Reading of data from scanner process "+c_unsigned(pid).ascii()+" finished.");

	// closing process
	STATUS("closing "+url);
	DEBUG("Waiting for scanner process "+c_unsigned(pid).ascii()+" to finish.");
	int wstat;
	DEBUG("Sending signal to process "+c_unsigned(pid).ascii()+".");
	kill(pid, SIGKILL);
	DEBUG("Sent signal to process "+c_unsigned(pid).ascii()+": "+(errno?"ok":strerror(errno))+".");
	int counter = 0;
	while (c_forker::wait_child_hang(&wstat) > 0)
	{
		datacomplete = datacomplete && WIFEXITED(wstat) && !WEXITSTATUS(wstat);
		counter++;
		DEBUG("Scanner process "+c_unsigned(pid).ascii()+" exited: "+convert::pstatus2print(wstat)+".");
	}
	DEBUG("KILLING FINISHED counter="+convert::si2str(counter)+" lastcode="+strerror(errno)+".");
	STATUS("");
	return datacomplete;
}
