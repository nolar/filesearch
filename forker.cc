#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "forker.h"

#include <iostream>
#include <iomanip>


vector<c_forker*> _forkers;


void forker_child (int)
{
	int wstat;
	pid_t pid;
	while ((pid = utils::wait_nohang(&wstat)) > 0) {
		for (vector<c_forker*>::iterator i = _forkers.begin(); i != _forkers.end(); i++)
		{
			(*i)->erase(pid, wstat);
		}
	}
}



c_forker::c_forker ()
{
	f_max_children = 1;
	f_children.clear();

	_forkers.push_back(this);
	utils::signal_block(SIGCHLD);
	utils::signal_catch(SIGCHLD, forker_child);
}

c_forker::c_forker (unsigned a_max_children)
{
	f_max_children = a_max_children;
	f_children.clear();

	_forkers.push_back(this);
	utils::signal_block(SIGCHLD);
	utils::signal_catch(SIGCHLD, forker_child);
}

c_forker::~c_forker ()
{
	for (vector<c_forker*>::iterator i = _forkers.begin(); i != _forkers.end(); i++)
	{
		if (*i == this)
		{
			_forkers.erase(i);
			break;
		}
	}
	if (_forkers.size() == 0)
	{
		utils::signal_uncatch(SIGCHLD);
		utils::signal_unblock(SIGCHLD);
	}
}



pid_t c_forker::fork (t_fork_func function, t_void_func init, t_void_func free)
{
	utils::signal_unblock(SIGCHLD);
	while (overmax()) utils::signal_pause();
	utils::signal_block(SIGCHLD);
	pid_t result = utils::fork(function, init, free);
	f_children.push_back(result);
	return result;
}

bool c_forker::erase (pid_t pid, int wstat)
{
	bool found = false;
	for (vector<pid_t>::iterator i = f_children.begin(); !found && i != f_children.end(); i++)
	{
		if (*i == pid)
		{
			f_children.erase(i);
			found = true;
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
