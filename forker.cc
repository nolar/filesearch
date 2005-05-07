#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <iomanip>
#include "forker.h"
#include "convert.h"
#include "io.h"


std::vector<c_forker*> _forkers;



void forker_signal_child (t_signal)
{
	int wstat;
	t_pident pid;
	while ((pid = c_forker::wait_child_nohang(&wstat)) > 0) {
		DEBUG("Controller process "+convert::pident2print(pid)+" exited: "+convert::pstatus2print(wstat)+".");
		for (std::vector<c_forker*>::iterator i = _forkers.begin(); i != _forkers.end(); i++)
			(*i)->erase(pid, wstat);
	}
}



c_forker::c_forker ()
{
	f_max_children = 1;
	f_children.clear();

	_forkers.push_back(this);
	signal_block(SIGCHLD);
	signal_catch(SIGCHLD, forker_signal_child);
}

c_forker::c_forker (unsigned a_max_children)
{
	f_max_children = a_max_children;
	f_children.clear();

	_forkers.push_back(this);
	signal_block(SIGCHLD);
	signal_catch(SIGCHLD, forker_signal_child);
}

c_forker::~c_forker ()
{
	for (std::vector<c_forker*>::iterator i = _forkers.begin(); i != _forkers.end(); i++)
	{
		if (*i == this)
		{
			_forkers.erase(i);
			break;
		}
	}
	if (_forkers.size() == 0)
	{
		signal_uncatch(SIGCHLD);
		signal_unblock(SIGCHLD);
	}
}



t_pident c_forker::fork (t_work_func work, t_void_func init, t_void_func free, t_catch_func catcher)
{
	// wating until there are some free lots for children
	//??? can not understand this. when unblocking is commented, smb-scanners consoumes
	//??? almost all CPU and often are in pfault stats (accord. to top) and the system
	//??? is lagging. when unblocking is uncommented - everying goes right. can not
	//??? understand this because signal_pause() should unblock all signals itself,
	//??? and why manual unblocking is required?
	signal_unblock(SIGCHLD);
	while (overmax()) signal_pause();
	signal_block(SIGCHLD);

	// forking
	t_pident pid = (t_pident) ::fork();
	int childstatus;
	switch (pid)
	{
		case -1:
			throw e_fork("Can not fork process.", errno, strerror(errno));
		case  0:
			try
			{
				if (init) init();
				childstatus = work();
				if (free) free();
			}
			catch (std::exception &e)
			{
				LOG("Exception in process id "+convert::pident2print(getpid())+": "+e.what());
				childstatus = catcher ? catcher(&e, NULL) : -1;
			}
			catch (e_basic &e)
			{
				LOG("Exception in process id "+convert::pident2print(getpid())+": "+e.what());
				childstatus = catcher ? catcher(NULL, &e) : -1;
			}
			catch (...)
			{
				LOG("Exception without type and message in process id "+convert::pident2print(getpid())+".");
				childstatus = catcher ? catcher(NULL, NULL) : -1;
			}
			_exit(childstatus);
		default:
			f_children.push_back(pid);
			return pid;
	};
}



bool c_forker::erase (t_pident pid, int wstat)
{
	bool found = false;
	for (std::vector<t_pident>::iterator i = f_children.begin(); i != f_children.end(); i++)
	{
		if (*i == pid)
		{
			f_children.erase(i);
			found = true;
			break;
		}
	}
	return found;
}



bool c_forker::empty ()
{
	return f_children.size() == 0;
}

bool c_forker::overmax ()
{
	return f_children.size() >= f_max_children;
}

unsigned c_forker::count ()
{
	return f_children.size();
}



t_pident c_forker::exec (char * cmd, char ** arg, char ** env, std::map<t_fd,t_fd> fds)
{
	t_pident pid = vfork();
	switch (pid)
	{
		case -1:
			throw e_fork("Can not fork process for executing.", errno, strerror(errno));
		case  0:
			for (std::map<int,int>::const_iterator i = fds.begin(); i != fds.end(); i++)
				io::fd_move(i->second, i->first);
			::execve(cmd, arg, env);
			LOG("Can not execute '"+cmd+"': "+strerror(errno));
			_exit(-1);
		default:
			return pid;
	};
}

t_pident c_forker::exec (std::string cmd, std::vector<std::string> arg, std::vector<std::string> env, std::map<t_fd,t_fd> fds)
{
	// preparing arg
	int argc = arg.size();
	char ** arg_ = new (char*) [argc + 1 + 1];
	arg_[0] = const_cast<char*>(cmd.c_str());
	for (int i = 0; i < argc; i++)
		arg_[i+1] = const_cast<char*>(arg[i].c_str());
	arg_[argc+1] = NULL;
	// preparing env
	int envc = env.size();
	char ** env_ = new (char*) [envc + 1];
	for (int i = 0; i < envc; i++)
		env_[i] = const_cast<char*>(env[i].c_str());
	env_[envc] = NULL;
	// executing in subprocess
	return exec(const_cast<char*>(cmd.c_str()), arg_, env_, fds);
}



void c_forker::signal_pause ()
{
	sigset_t ss;
	::sigemptyset(&ss);
	::sigsuspend(&ss);
}

void c_forker::signal_block (t_signal signal)
{
	sigset_t ss;
	::sigemptyset(&ss);
	::sigaddset(&ss, signal);
	::sigprocmask(SIG_BLOCK, &ss, NULL);
}

void c_forker::signal_unblock (t_signal signal)
{
	sigset_t ss;
	::sigemptyset(&ss);
	::sigaddset(&ss, signal);
	::sigprocmask(SIG_UNBLOCK, &ss, NULL);
}

void c_forker::signal_catch (t_signal signal, t_signal_func function)
{
	struct sigaction sa;
	sa.sa_handler = function;
	sa.sa_flags = 0;
	::sigemptyset(&sa.sa_mask);
	::sigaction(signal, &sa, NULL);
}

void c_forker::signal_uncatch (t_signal signal)
{
	signal_catch(signal, SIG_DFL);
}

void c_forker::signal_ignore (t_signal signal)
{
	signal_catch(signal, SIG_IGN);
}



t_pident c_forker::wait_nohang (t_pident pid, int * wstat)
{
	return ::waitpid(pid, wstat, WNOHANG);
}

t_pident c_forker::wait_hang (t_pident pid, int * wstat)
{
	return ::waitpid(pid, wstat, 0);
}

t_pident c_forker::wait_child_nohang (int * wstat)
{
	return wait_nohang(-1, wstat);
}

t_pident c_forker::wait_child_hang (int * wstat)
{
	return wait_hang(-1, wstat);
}

t_pident c_forker::wait_group_nohang (int * wstat)
{
	return wait_nohang(0, wstat);
}

t_pident c_forker::wait_group_hang (int * wstat)
{
	return wait_hang(0, wstat);
}

