#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#include "fdstream"
#include "thread.h"
#include "database.h"
#include "forker.h"
#include "utils.h"
#include "options.h"

extern c_database * database;
extern c_forker * forker;

void thread_init ()
{
	database = database->duplicate(); //!!! exscentric way to make copy of existing database. better to replace it with create_database(), which will use connection parameters to determine type of database.
	forker = NULL;
	utils::signal_uncatch(SIGCHLD);
	utils::signal_unblock(SIGCHLD);
	utils::signal_ignore(SIGPIPE);
}

void thread_free ()
{
	delete database;
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
	DEBUG("Creating pipes to scanner process for ip='"+utils::inet_ntoa(request.address())+"',share='"+request.share()+"',username='"+request.username()+"'.");
	int ifd[2]; if (pipe(ifd) == -1) throw e_basic("can not create in pipe!!!", errno, strerror(errno));//e_basic???!!!
	DEBUG("Created pipe "+utils::ultostr(ifd[0])+"<->"+utils::ultostr(ifd[1])+" for task stream of scanner process for ip='"+utils::inet_ntoa(request.address())+"',share='"+request.share()+"',username='"+request.username()+"'.");
	int ofd[2]; if (pipe(ofd) == -1) throw e_basic("can not create out pipe!!!", errno, strerror(errno));//e_basic???!!!
	DEBUG("Created pipe "+utils::ultostr(ofd[0])+"<->"+utils::ultostr(ofd[1])+" for data stream of scanner process for ip='"+utils::inet_ntoa(request.address())+"',share='"+request.share()+"',username='"+request.username()+"'.");

	// executing scan process with substituted fds
	map<int,int> fds;
	vector<string> arg;
	vector<string> env;
	fds[options::fd_task] = ifd[0];
	fds[options::fd_data] = ofd[1];
//???	fds[options::fd_debug] = options::fd_null;
	arg.push_back(utils::inet_ntoa(request.address()));
	arg.push_back(request.share());
	pid_t pid = utils::exec(command.c_str(), arg, env, fds);
	DEBUG("Created scanner process "+utils::ultostr(pid)+" for ip='"+utils::inet_ntoa(request.address())+"',share='"+request.share()+"',username='"+request.username()+"'.");
	close(ifd[0]); close(ofd[1]);

	// sending parameters to scanner process
	t_stream_status status;
	vector<string> task;
	task.push_back(utils::inet_ntoa(request.address()));
	task.push_back(request.share());
	task.push_back(request.username());
	task.push_back(request.password());
	task.push_back(request.workgroup());
	task.push_back(request.selfname());
	task.push_back(utils::ultostr(request.timeout()));
	task.push_back(utils::ultostr(request.depth()));
	DEBUG("Writing task for scanner process "+utils::ultostr(pid)+".");
	utils::writeblock(ifd[1], NULL, &status, options::terminator, task);
	if (status) throw e_basic("Can not write task to scanner process "+utils::ultostr(pid)+" status "+utils::ultostr(status)+".");

	// reading and handling output of scanner process
	timeval timer;
	bool datacomplete = false;
	do {
		DEBUG("Getting reply from scanner process "+utils::ultostr(pid)+".");
		timer.tv_sec = request.timeout(); timer.tv_usec = 0;
		vector<string> data = utils::readblock(ofd[0], &timer, &status, options::terminator, true, true);
		string action; if (data.size()) { action = data[0]; data.erase(data.begin()); }
		if (status) DEBUG("Can not read data from scanner process "+utils::ultostr(pid)+" status "+utils::ultostr(status)+".");
		else        DEBUG("Got action='"+action+"' and "+utils::ultostr(data.size())+" data lines from scanner process "+utils::ultostr(pid)+".");
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
				c_path path  = utils::string2path(data[0]);
				action_resource(request, path);
			} else
			if (action_dir      && action == options::action_code_for_dir      && data.size() >= 3)
			{
				c_path path  = utils::string2path(data[0]);
				time_t ctime = utils::string2time(data[1]);
				time_t mtime = utils::string2time(data[2]);
				c_fileinfo fileinfo(path, true, 0, ctime, mtime);
				action_dir(request, fileinfo);
			} else
			if (action_file     && action == options::action_code_for_file     && data.size() >= 4)
			{
				c_path path  = utils::string2path(data[0]);
				size_t size  = utils::string2size(data[1]);
				time_t ctime = utils::string2time(data[2]);
				time_t mtime = utils::string2time(data[3]);
				c_fileinfo fileinfo(path, false, size, ctime, mtime);
				action_file(request, fileinfo);
			} else
			if (action_enter    && action == options::action_code_for_enter    && data.size() >= 1)
			{
				c_path path = utils::string2path(data[0]);
				action_enter(request, path);
			} else
			if (action_leave    && action == options::action_code_for_leave    && data.size() >= 1)
			{
				c_path path = utils::string2path(data[0]);
				action_leave(request, path);
			} else
			{/* unkown action or known action with no handler */}
		}
	} while (!status);
	close(ifd[1]); close(ofd[0]);
	DEBUG("Reading of data from scanner process "+utils::ultostr(pid)+" finished.");

	// closing process
	DEBUG("Waiting for scanner process "+utils::ultostr(pid)+" to finish.");
	int wstat;
	DEBUG("Sending signal to pid "+utils::ultostr(pid)+".");
	int c = kill(pid, SIGKILL);
	DEBUG("Sent signal to pid "+utils::ultostr(pid)+" code "+utils::ultostr(c)+":"+utils::ultostr(errno)+":"+strerror(errno)+".");
	bool counter = 0;
	while (utils::wait_hang(&wstat) > 0)
	{
		datacomplete = datacomplete && WIFEXITED(wstat) && !WEXITSTATUS(wstat);
		counter++;
		if (WIFEXITED  (wstat) && !WEXITSTATUS(wstat)) DEBUG("Scanner process "+utils::ultostr(pid)+" exited successfully."); else
		if (WIFEXITED  (wstat)                       ) DEBUG("Scanner process "+utils::ultostr(pid)+" exited with code "+utils::ultostr(WEXITSTATUS(wstat))+"."); else
		if (WIFSIGNALED(wstat) &&  WCOREDUMP(wstat)  ) DEBUG("Scanner process "+utils::ultostr(pid)+" core dumped on signal "+utils::ultostr(WTERMSIG(wstat))+"."); else
		if (WIFSIGNALED(wstat)                       ) DEBUG("Scanner process "+utils::ultostr(pid)+" exited on signal "+utils::ultostr(WTERMSIG(wstat))+"."); else
		if (WIFSTOPPED (wstat)                       ) DEBUG("Scanner process "+utils::ultostr(pid)+" stopped on signal "+utils::ultostr(WSTOPSIG(wstat))+"."); else
							       DEBUG("Scanner process "+utils::ultostr(pid)+" exited with wstat "+utils::ultostr(wstat)+".");
	}
	DEBUG("KILLING FINISHED counter="+utils::ultostr(counter)+" lastcode="+utils::ultostr(errno)+":"+strerror(errno)+".");
	return datacomplete;
}
