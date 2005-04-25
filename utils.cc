#include "utils.h"
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <signal.h>

unsigned long utils::inet_aton (string address)
{
	//!!! add more checks for valid address here
	//!!! and throw e_address if can't convert
	string::size_type pos, pre = 0;
	string s[4];
	unsigned long a[4], result = 0;
	bool enough = false;
	for (int i = 0; i < 4 && !enough; i++, pre = pos + 1)
	{
		pos = address.find(".", pre);
		enough = pos == string::npos;
		s[i] = enough ? address.substr(pre) : address.substr(pre, pos-pre);
	}
	for (int i = 0; i < 4; i++)
	{
		a[i] = strtoul(s[i]);
		result = (result << 8) | (a[i] & 0xff);
	}
	return result;
}

string utils::inet_ntoa (unsigned long address)
{
	string result;
	result += ultostr((address >> 24) & 0xff) + ".";
	result += ultostr((address >> 16) & 0xff) + ".";
	result += ultostr((address >>  8) & 0xff) + ".";
	result += ultostr((address      ) & 0xff);
	return result;
}

/*!!!!
string utils::iconv_convert (iconv_t handle, string s)
{
//	printf("in0\n");
	string result;
	string::size_type len = s.length();
	string::size_type slength = len  ;
	string::size_type dlength = len*2;
//	printf("in1: %d\n", len);
//	printf("in2: %d, %d\n", slength, dlength);
	string::value_type * sbuffer = new string::value_type [slength+1];
	string::value_type * dbuffer = new string::value_type [dlength+1], *dpointer = dbuffer;
	const string::value_type * cpointer = const_cast<const string::value_type*>(sbuffer);
//	printf("in3: %p, %p\n", sbuffer, dbuffer);
//	printf("in4:\n");
	strncpy(sbuffer, s.c_str(), slength);
	sbuffer[slength] = 0;
//	printf("in5: %s\n", sbuffer);

	::iconv(handle, NULL, NULL, NULL, NULL);
//	size_t res = ::iconv(handle, &cpointer, &slength, &dpointer, &dlength);
	::iconv(handle, &cpointer, &slength, &dpointer, &dlength);
	*dpointer = 0;
//	printf("in6: %d (%d,%d)\n", res, slength, dlength);
///	if (res != (size_t)-1)
///	{
//		printf("in7: %s\n", dbuffer);
		result = dbuffer;
///	}
//	printf("in8: %s\n", result.c_str());
	delete[] sbuffer;
	delete[] dbuffer;
//	printf("in9: %x, %x\n", sbuffer, dbuffer);
	return result;
}

iconv_t iconv_utf8_koi8r; bool iconv_utf8_koi8r_ready = false;
iconv_t iconv_koi8r_utf8; bool iconv_koi8r_utf8_ready = false;

string utils::utf8_koi8r (string s)
{
	if (!iconv_utf8_koi8r_ready)
	{
		iconv_utf8_koi8r = ::iconv_open("KOI8-R//IGNORE", "UTF-8");
		if (iconv_utf8_koi8r == (iconv_t) -1)
		{
			throw e_iconv("Can not create iconv handle for UTF-8 to KOI8-R.", strerror(errno));
		}
		iconv_utf8_koi8r_ready = true;
	}
	return iconv_convert(iconv_utf8_koi8r, s);
}

string utils::koi8r_utf8 (string s)
{
	if (!iconv_koi8r_utf8_ready)
	{
		iconv_koi8r_utf8 = ::iconv_open("UTF-8//IGNORE", "KOI8-R");
		if (iconv_koi8r_utf8 == (iconv_t) -1)
		{
			throw e_iconv("Can not create iconv handle for KOI8-R to UTF-8.", strerror(errno));
		}
		iconv_koi8r_utf8_ready = true;
	}
	return iconv_convert(iconv_koi8r_utf8, s);
}

void utils::iconv_free ()
{
	if (iconv_koi8r_utf8_ready) { ::iconv_close(iconv_koi8r_utf8); iconv_koi8r_utf8_ready = false; }
	if (iconv_utf8_koi8r_ready) { ::iconv_close(iconv_utf8_koi8r); iconv_utf8_koi8r_ready = false; }
}
*/


e_address::e_address (string address)
{
	f_binary = false;
	f_string_data = address;
	f_binary_data = 0;
}

e_address::e_address (unsigned long address)
{
	f_binary = true;
	f_string_data = "";
	f_binary_data = address;
}

e_address::~e_address () throw ()
{
}

string e_address::message ()
{
	return f_binary ?
		(string() + "Can not convert address " + utils::ultostr(f_binary_data) + " to its textual form.") :
		(string() + "Can not convert address '" + f_string_data + "' to its binary form.");
}




string utils::ultostr (unsigned long value)
{
	string::value_type * buffer = (string::value_type*) calloc(1024, sizeof(string::value_type));
	sprintf(buffer, "%U", value);
	string result = buffer;
	free(buffer);
	return result;
}

unsigned long utils::strtoul (string value)
{
	return ::strtoul(value.c_str(), NULL, 10);
}

c_path utils::string2path (string s)
{
	string::size_type idx, pos, prev = (string::size_type) -1;
	c_path result;
	do {
		idx = (prev+1 >= s.length()) ? string::npos : s.find('/', prev+1);
		pos = (idx == string::npos) ? s.length() : idx;
		string item = s.substr(prev+1, pos-(prev+1));
		if (!item.empty()) result.push_back(item);
		prev = pos;
	} while (idx != string::npos);
	return result;
}

string utils::path2string (c_path path)
{
	string result;
	for (c_path::const_iterator i = path.begin(); i != path.end(); i++)
	{
		result = result + '/' + *i;
	}
	return result;
}



string utils::getline (istream * stream, string on_eof)
{
	if (stream->eof() || !stream->good()) return on_eof;
	const int stream_buffer_size = 1024;
	char * buffer = new char [stream_buffer_size+1];
	memset(buffer, 0, stream_buffer_size+1);
	stream->getline(buffer, stream_buffer_size);
	string result = buffer;
	delete[] buffer;
	return result;
}

string utils::readline (int fd, timeval * timer, t_stream_status * status, string::value_type terminator, bool strict)
{
	timeval time_start, time_stop, time_now, time_left;
	if (timer)
	{
		if (timer->tv_sec < 0 || timer->tv_usec < 0 || (timer->tv_sec == 0 && timer->tv_usec == 0)) return "";
		gettimeofday(&time_start, NULL);
		timeradd(&time_start, timer, &time_stop);
	}
	if (status)
	{
		*status = stream_status_ok;
	}
	string result;
	do {
		// calculating time left for io operations
		if (timer)
		{
			gettimeofday(&time_now, NULL);
			timersub(&time_stop, &time_now, &time_left);
		}
		int code;
		// waiting for data from file during specified time
		fd_set fdset;
		FD_ZERO(&fdset);
		FD_SET(fd, &fdset);
		code = select(1, &fdset, NULL, NULL, timer?&time_left:NULL);
		if (code ==  0) { if (status) *status = stream_status_timeout; if (strict) result = ""; break; }
		if (code == -1) { if (status) *status = stream_status_error  ; throw e_stream("Can not retrieve read status of fd "+utils::ultostr(fd)+".", errno, strerror(errno)); }
		// reading data from file if they are there
		string::value_type buf;
		code = read(fd, &buf, sizeof(buf));
		if (code ==  0) { if (status) *status = stream_status_eof    ; if (strict) result = ""; break; }
		if (code == -1) { if (status) *status = stream_status_error  ; throw e_stream("Can not read from fd "+utils::ultostr(fd)+".", errno, strerror(errno)); }
		if (buf == terminator) break;
		result += buf;
	} while (true);
	gettimeofday(&time_now, NULL);
	timersub(&time_stop, &time_now, timer);
	if (timer->tv_sec < 0 || timer->tv_usec < 0) timerclear(timer);
	return result;
}

vector<string> utils::readblock (int fd, timeval * timer, t_stream_status * status, string::value_type terminator, bool strict, bool skipempty, unsigned mincount, unsigned maxcount)
{
	vector<string> result;
	do {
		t_stream_status substatus;
		string line = readline(fd, timer, &substatus, terminator, strict);
		if (status) *status = substatus;
		if (line.empty() && (!mincount || result.size() >= mincount)) break;
		if (!line.empty() || !skipempty) result.push_back(line);
		if (substatus    || ( maxcount && result.size() >= maxcount)) break;
	} while (true);
	return result;
}

void utils::writeline (int fd, timeval * timer, t_stream_status * status, string::value_type terminator, string value)
{
	timeval time_start, time_stop, time_now, time_left;
	if (timer)
	{
		if (timer->tv_sec < 0 || timer->tv_usec < 0 || (timer->tv_sec == 0 && timer->tv_usec == 0)) return;
		gettimeofday(&time_start, NULL);
		timeradd(&time_start, timer, &time_stop);
	}
	if (status)
	{
		*status = stream_status_ok;
	}
	string::size_type pos = 0; value += terminator;
	do {
		// calculating time left for io operations
		if (timer)
		{
			gettimeofday(&time_now, NULL);
			timersub(&time_stop, &time_now, &time_left);
		}
		int code;
		// waiting for data from file during specified time
		fd_set fdset;
		FD_ZERO(&fdset);
		FD_SET(fd, &fdset);
		code = select(1, NULL, &fdset, NULL, timer?&time_left:NULL);
		if (code ==  0) { if (status) *status = stream_status_timeout; break; }
		if (code == -1) { if (status) *status = stream_status_error  ; throw e_stream("Can not retrieve write status of fd "+utils::ultostr(fd)+".", errno, strerror(errno)); }
		// reading data from file if they are there
		string buf = value.substr(pos); int bufsize = buf.length();
		code = write(fd, buf.c_str(), bufsize);
		if (code == -1) { if (status) *status = stream_status_error  ; throw e_stream("Can not read from fd "+utils::ultostr(fd)+".", errno, strerror(errno)); }
		if (code == bufsize) break;
		pos += code;
	} while (true);
	gettimeofday(&time_now, NULL);
	timersub(&time_stop, &time_now, timer);
	if (timer->tv_sec < 0 || timer->tv_usec < 0) timerclear(timer);
}

void utils::writeblock (int fd, timeval * timer, t_stream_status * status, string::value_type terminator, vector<string> value)
{
	value.push_back(string());
	for (vector<string>::const_iterator i = value.begin(); i != value.end(); i++)
	{
		t_stream_status substatus;
		writeline(fd, timer, &substatus, terminator, *i);
		if (status) *status = substatus;
		if (substatus) break;
	}
}



pid_t utils::fork (t_fork_func function, t_void_func init, t_void_func free)
{
	pid_t result = ::fork();
	int status;
	switch (result)
	{
		case -1:
			throw e_fork("Can not fork process.", errno, strerror(errno));
		case  0:
			try
			{
				if (init) init();
				status = function();
				if (free) free();
			}
			catch (exception &e)
			{
				status = 1;
			}
			catch (...)
			{
				status = 1;
			}
			_exit(status);
		default:
			return result;
	};
}

pid_t utils::exec (char * cmd, char ** arg, char ** env, int stdin, int stdout, int stderr)
{
	pid_t result = vfork();
	switch (result)
	{
		case -1:
			throw e_fork("Can not fork process for executing.", errno, strerror(errno));
		case  0:
			fd_move(stdin , 0);
			fd_move(stdout, 1);
			fd_move(stderr, 2);
			execve(cmd, arg, env);
			//!!! log error here!
			_exit(1);
//???			throw e_fork(string() + "Can not execute file '" + cmd + "'.", errno, strerror(errno));
		default:
			return result;
	};
	
}

pid_t utils::exec (string cmd, vector<string> arg, vector<string> env, int stdin, int stdout, int stderr)
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
	return exec(const_cast<char*>(cmd.c_str()), arg_, env_, stdin, stdout, stderr);
}



void utils::fd_copy (int oldfd, int newfd)
{
	if ((oldfd == -1) || (newfd == -1) || (oldfd == newfd)) return;
	if (fcntl(oldfd, F_GETFL, 0) == -1)
		throw e_fork("Can not copy fd "+utils::ultostr(oldfd)+" to fd "+utils::ultostr(newfd)+" because source fd does not exist.", errno, strerror(errno));
	close(newfd);
	if (fcntl(oldfd, F_DUPFD, newfd) == -1)
		throw e_fork("Can not copy fd "+utils::ultostr(oldfd)+" to fd "+utils::ultostr(newfd)+" because duping failed.", errno, strerror(errno));
}

void utils::fd_move (int oldfd, int newfd)
{
	fd_copy(oldfd, newfd);
	close(oldfd);
}



void utils::signal_pause ()
{
	sigset_t ss;
	sigemptyset(&ss);
	sigsuspend(&ss);
}

void utils::signal_block (t_signal signal)
{
	sigset_t ss;
	sigemptyset(&ss);
	sigaddset(&ss, signal);
	sigprocmask(SIG_BLOCK, &ss, NULL);
}

void utils::signal_unblock (t_signal signal)
{
	sigset_t ss;
	sigemptyset(&ss);
	sigaddset(&ss, signal);
	sigprocmask(SIG_UNBLOCK, &ss, NULL);
}

void utils::signal_catch (t_signal signal, t_signal_func function)
{
	struct sigaction sa;
	sa.sa_handler = function;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(signal, &sa, NULL);
}

void utils::signal_uncatch (t_signal signal)
{
	signal_catch(signal, SIG_DFL);
}

void utils::signal_ignore (t_signal signal)
{
	signal_catch(signal, SIG_IGN);
}



pid_t utils::wait_nohang (int * wstat)
{
	return waitpid(-1, wstat, WNOHANG);
}

pid_t utils::wait_hang (int * wstat)
{
	return waitpid(-1, wstat, 0);
}

pid_t utils::wait_nohang (pid_t pid, int * wstat)
{
	return waitpid(pid, wstat, WNOHANG);
}

pid_t utils::wait_hang (pid_t pid, int * wstat)
{
	return waitpid(pid, wstat, 0);
}
