#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include "thread.h"
#include "database.h"
#include "forker.h"
#include "io.h"
#include "options.h"
#include "convert.h"

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
	delete database;
	return 1;
}

bool thread_wrap (string command, c_request request,
	t_thread_wrap_action_resource action_resource,
	t_thread_wrap_action_dir      action_dir,
	t_thread_wrap_action_file     action_file,
	t_thread_wrap_action_enter    action_enter,
	t_thread_wrap_action_leave    action_leave,
	t_thread_wrap_action_start    action_start)
{
	// creating fd for stdin & stdout of scan process
	DEBUG("Creating pipes to scanner process for ip='"+convert::ipaddr2print(request.address())+"',share='"+request.share()+"',username='"+request.username()+"'.");
	int ifd[2]; if (pipe(ifd) == -1) throw e_basic("can not create in pipe!!!", errno, strerror(errno));//e_basic???!!!
	DEBUG("Created pipe "+convert::fd2print(ifd[0])+"<->"+convert::fd2print(ifd[1])+" for task stream of scanner process for ip='"+convert::ipaddr2print(request.address())+"',share='"+request.share()+"',username='"+request.username()+"'.");
	int ofd[2]; if (pipe(ofd) == -1) throw e_basic("can not create out pipe!!!", errno, strerror(errno));//e_basic???!!!
	DEBUG("Created pipe "+convert::fd2print(ofd[0])+"<->"+convert::fd2print(ofd[1])+" for data stream of scanner process for ip='"+convert::ipaddr2print(request.address())+"',share='"+request.share()+"',username='"+request.username()+"'.");

	// executing scan process with substituted fds
	map<int,int> fds;
	vector<string> arg;
	vector<string> env;
	fds[io::fd_task] = ifd[0];
	fds[io::fd_data] = ofd[1];
//???	fds[io::fd_debug] = io::fd_null;
	arg.push_back(convert::ipaddr2print(request.address()));
	arg.push_back(request.share());
	t_pident pid = c_forker::exec(command, arg, env, fds);
	DEBUG("Created scanner process "+convert::pident2print(pid)+" for ip='"+convert::ipaddr2print(request.address())+"',share='"+request.share()+"',username='"+request.username()+"'.");
	close(ifd[0]); close(ofd[1]);

	// sending parameters to scanner process
	t_stream_status status;
	vector<string> task;
	task.push_back(convert::ipaddr2print(request.address()));
	task.push_back(request.share());
	task.push_back(request.username());
	task.push_back(request.password());
	task.push_back(request.workgroup());
	task.push_back(request.selfname());
	task.push_back(convert::timeout2system(request.timeout()));
	task.push_back(convert::timeout2system(request.depth()));
	DEBUG("Writing task for scanner process "+convert::pident2print(pid)+".");
	io::writeblock(ifd[1], NULL, &status, options::terminator, task); //!!! make it woth timeout 1 sec
	if (status) throw e_basic("Can not write task to scanner process "+convert::pident2print(pid)+" status "+convert::ui2str(status)+"."); //!!! ui2str -> stream2print

	// reading and handling output of scanner process
	timeval timer;
	bool datacomplete = false;
	do {
		DEBUG("Getting reply from scanner process "+convert::pident2print(pid)+".");
		timer.tv_sec = request.timeout(); timer.tv_usec = 0;
		vector<string> data = io::readblock(ofd[0], &timer, &status, options::terminator, true, true);
		string action; if (data.size()) { action = data[0]; data.erase(data.begin()); }
		if (status) DEBUG("Can not read data from scanner process "+convert::pident2print(pid)+" status "+convert::ui2str(status)+"."); //!!! ui2str -> stream2print
		else        DEBUG("Got action='"+action+"' and "+convert::ui2str(data.size())+" data lines from scanner process "+convert::pident2print(pid)+".");
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
	DEBUG("Reading of data from scanner process "+convert::pident2print(pid)+" finished.");

	// closing process
	DEBUG("Waiting for scanner process "+convert::pident2print(pid)+" to finish.");
	int wstat;
	DEBUG("Sending signal to process "+convert::pident2print(pid)+".");
	kill(pid, SIGKILL);
	DEBUG("Sent signal to process "+convert::pident2print(pid)+": "+(errno?"ok":strerror(errno))+".");
	int counter = 0;
	while (c_forker::wait_child_hang(&wstat) > 0)
	{
		datacomplete = datacomplete && WIFEXITED(wstat) && !WEXITSTATUS(wstat);
		counter++;
		DEBUG("Scanner process "+convert::pident2print(pid)+" exited: "+convert::pstatus2print(wstat)+".");
	}
	DEBUG("KILLING FINISHED counter="+convert::si2str(counter)+" lastcode="+strerror(errno)+".");
	return datacomplete;
}
