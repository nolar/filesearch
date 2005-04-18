#ifndef _FILESEARCH_FORKER_
#define _FILESEARCH_FORKER_

#include <string>
#include <vector>
#include <map>
#include "utils.h"

using namespace std;

class c_forker
{
private:
	unsigned f_max_children;
	vector<pid_t> f_children;
protected:
public:
	c_forker ();
	c_forker (unsigned a_max_children);
	virtual ~c_forker ();

	pid_t fork (t_fork_func function, t_void_func init = NULL, t_void_func free = NULL);
	bool erase (pid_t pid, int wstat);
	bool empty ();
	bool overmax ();
	unsigned count ();
};

#endif
