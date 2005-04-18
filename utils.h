#ifndef _FILESEARCH_UTILS_FUNCTIONS_
#define _FILESEARCH_UTILS_FUNCTIONS_

#include <string>
#include <vector>
#include <iconv.h>
#include "e_iconv.h"
#include "e_fork.h"

using namespace std;

typedef unsigned int t_id;
typedef unsigned long t_ipaddr;
typedef unsigned int  t_ipmask;
typedef unsigned int  t_ipport;
typedef enum{proto_unknown, proto_smb, proto_ftp, proto_http} t_proto;
typedef vector<string> c_path;

typedef int  (*t_fork_func) ();
typedef void (*t_void_func) ();
typedef void (*t_signal_func) (int);
typedef int t_signal;

class utils
{
private:
protected:
	static string iconv_convert (iconv_t handle, string s);
public:
	static unsigned long inet_aton (string address);
	static string inet_ntoa (unsigned long address);
	static string ultostr (unsigned long value);
	static unsigned long strtoul (string value);
	static c_path string2path (string s);
	static string path2string (c_path path);

	static string utf8_koi8r (string s);
	static string koi8r_utf8 (string s);
	static void iconv_free ();

	// streaming routies
	static string getline(istream * stream);

	// forking routines
	static pid_t fork (t_fork_func function, t_void_func init = NULL, t_void_func free = NULL);
	static pid_t exec (char * cmd, char ** arg, char ** env, int stdin = -1, int stdout = -1, int stderr = -1);
	static pid_t exec (string cmd, vector<string> arg, vector<string> env, int stdin = -1, int stdout = -1, int stderr = -1);

	// file descriptor handling
	static void fd_copy (int oldfd, int newfd);
	static void fd_move (int oldfd, int newfd);

	// signal handling
	static void signal_pause   ();
	static void signal_block   (t_signal signal);
	static void signal_unblock (t_signal signal);
	static void signal_catch   (t_signal signal, t_signal_func function);
	static void signal_uncatch (t_signal signal);
	static void signal_ignore  (t_signal signal);

	// children handling
	static pid_t wait_nohang (int * wstat);
	static pid_t wait_hang   (int * wstat);
	static pid_t wait_nohang (pid_t pid, int * wstat);
	static pid_t wait_hang   (pid_t pid, int * wstat);
};


class e_address : public exception
{
private:
	bool f_binary;
	string f_string_data;
	unsigned long f_binary_data;
protected:
public:
	e_address (unsigned long address);
	e_address (string address);
	virtual ~e_address () throw ();
	virtual string message ();
};

#endif
