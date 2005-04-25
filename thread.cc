#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sys/types.h>
#include <sys/wait.h>
#include "fdstream"
#include "thread.h"
#include "database.h"
#include "forker.h"
#include "utils.h"

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

void thread_wrap (string command, c_request request, t_thread_wrap_topaction topaction, t_thread_wrap_subaction subaction, t_thread_wrap_flushaction flushaction)
{
	// creating fd for stdin & stdout of scan process
	int isfd[2]; if (pipe(isfd) == -1) throw e_basic("can not create in pipe!!!");//e_basic???!!!
	int osfd[2]; if (pipe(osfd) == -1) throw e_basic("can not create out pipe!!!");//e_basic???!!!

	// executing scan process with substituted fds
	vector<string> empty;
	vector<string> abc;//!!! remove abc. replace to 'empty'
	abc.push_back(utils::inet_ntoa(request.address()));
	pid_t cmdpid = utils::exec(command.c_str(), abc, empty, isfd[0], osfd[1]);

	// creating streams to work with fds on our side
	close(isfd[0]); close(osfd[1]);
	fdostream is(isfd[1]);
	fdistream os(osfd[0]);

	// sending parameters to scanner process
	is <<        utils::inet_ntoa(request.address())	<< endl;
	is <<        request.share()		<< endl;
	is <<        request.username()		<< endl;
	is <<        request.password()		<< endl;
	is <<        request.workgroup()	<< endl;
	is <<        request.selfname()		<< endl;
	is << dec << request.timeout()		<< endl;
	is << dec << request.depth()            << endl;

	// reading and handling output of scanner process
	bool haveanydata = false;
	do {
		string path_s      = utils::getline(&os);
		if (!path_s.empty() && path_s[0] == '#')
		{
			path_s.erase(0, 1);
			if (!os.eof() || (!path_s.empty()))
			{
				if (flushaction)
				{
					c_path path = utils::string2path(path_s);
					flushaction(request, path);
				}
			}
		} else
		if (!path_s.empty() && path_s[0] == '/')
		{
			string container_s = utils::getline(&os);
			string size_s      = utils::getline(&os);
			string ctime_s     = utils::getline(&os);
			string mtime_s     = utils::getline(&os);
			if (!os.eof() || (!path_s.empty() && !container_s.empty() && !size_s.empty() && !ctime_s.empty() && !mtime_s.empty()))
			{
				if (!haveanydata && topaction)
				{
					topaction(request);
					haveanydata = true;
				}
				if (subaction)
				{
					c_fileinfo fileinfo(path_s, container_s, size_s, ctime_s, mtime_s);
					subaction(request, fileinfo);
				}
			}
		}
		if (!os.eof()) os >> ws;
	} while (!os.eof());
	close(isfd[1]); close(osfd[0]);

	// closing process
	int wstat;
	waitpid(cmdpid, &wstat, 0);
	if (WIFEXITED(wstat))
	{
		cerr << "Process id " << cmdpid << " for ip " << utils::inet_ntoa(request.address()) << "/" << request.share() << " exited normally with code " << WEXITSTATUS(wstat) << endl;
	} else
	if (WIFSIGNALED(wstat) && WCOREDUMP(wstat))
	{
		cerr << "Process id " << cmdpid << " for ip " << utils::inet_ntoa(request.address()) << "/" << request.share() << " dunped its core on signal " << WTERMSIG(wstat) << endl;
	} else
	if (WIFSIGNALED(wstat))
	{
		cerr << "Process id " << cmdpid << " for ip " << utils::inet_ntoa(request.address()) << "/" << request.share() << " exited on signal " << WTERMSIG(wstat) << endl;
	} else
	{
		cerr << "Process id " << cmdpid << " for ip " << utils::inet_ntoa(request.address()) << "/" << request.share() << " exited with wstat " << dec << wstat << "(" << hex << wstat << dec << ")" << endl;
	}
}
