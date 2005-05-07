#ifndef _FILESEARCH_FORKER_
#define _FILESEARCH_FORKER_

#include <exception>
#include <string>
#include <vector>
#include <map>
#include "config.h"
#include "typedefs.h"
#include "e_fork.h"
#include "e_basic.h"

typedef int t_signal;
typedef void (*t_signal_func) (t_signal);
typedef int  (*t_work_func) ();
typedef void (*t_void_func) ();
typedef int  (*t_catch_func) (std::exception*, e_basic*);

class c_forker
{
private:
	unsigned f_max_children;
	std::vector<t_pident> f_children;
protected:
public:
	c_forker ();
	c_forker (unsigned a_max_children);
	virtual ~c_forker ();

	// childrean management
	t_pident fork (t_work_func work, t_void_func init = NULL, t_void_func free = NULL, t_catch_func catcher = NULL);
	bool erase (t_pident pid, int wstat);
	bool empty ();
	bool overmax ();
	unsigned count ();

	// executors
	static t_pident exec (char *      cmd, char **                  arg, char **                  env, std::map<t_fd,t_fd> fds);
	static t_pident exec (std::string cmd, std::vector<std::string> arg, std::vector<std::string> env, std::map<t_fd,t_fd> fds);

	// signal handling
	static void signal_pause   ();
	static void signal_block   (t_signal signal);
	static void signal_unblock (t_signal signal);
	static void signal_catch   (t_signal signal, t_signal_func function);
	static void signal_uncatch (t_signal signal);
	static void signal_ignore  (t_signal signal);

	// children handling
	static t_pident wait_nohang (t_pident pid, int * wstat);
	static t_pident wait_hang   (t_pident pid, int * wstat);
	static t_pident wait_child_nohang (int * wstat);
	static t_pident wait_child_hang   (int * wstat);
	static t_pident wait_group_nohang (int * wstat);
	static t_pident wait_group_hang   (int * wstat);
};

#endif
