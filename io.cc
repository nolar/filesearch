#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/select.h>
#include "io.h"
#include "convert.h"



t_fd io::fd_task  = 0;
t_fd io::fd_data  = 3;
t_fd io::fd_log   = 2;
t_fd io::fd_debug = 1;
t_fd io::fd_null  = -1;
char * io::fd_null_file = "/dev/null";

std::string::value_type io::log_terminator  = '\n';
std::string             io::log_time_format = "%Y-%m-%d %H:%M:%S";
int                     io::log_pid_length  = 5;



std::string io::_read (t_fd fd, timeval * timer, t_stream_status * status, int * error, std::string::value_type terminator, bool strict, bool skipbefore)
{
	std::string result;
	t_stream_status status_ = stream_status_ok; int error_ = 0;
	timeval time_start, time_stop, time_now, time_left;
	// prepairing timers
	if (timer)
	{
		if (timer->tv_sec < 0 || timer->tv_usec < 0 || (timer->tv_sec == 0 && timer->tv_usec == 0)) { status_ = stream_status_timeout; }
		gettimeofday(&time_start, NULL);
		timeradd(&time_start, timer, &time_stop);
	}
	// checking if fd exists
	if (!fd_ok(fd)) { status_ = stream_status_nofd; error_ = errno; }
	// reading
	while (!status_)
	{
		int code;
		// calculating time left for io operations
		if (timer)
		{
			gettimeofday(&time_now, NULL);
			timersub(&time_stop, &time_now, &time_left);
			if (time_left.tv_sec < 0 || time_left.tv_usec < 0 || (time_left.tv_sec == 0 && time_left.tv_usec ==0))
				{ status_ = stream_status_timeout; break; }
		}
		// waiting for data from file during specified time
		fd_set fdset;
		FD_ZERO(&fdset);
		FD_SET(fd, &fdset);
		code = ::select(fd+1, &fdset, NULL, NULL, timer ? &time_left : NULL);
		if (code ==  0) { status_ = stream_status_timeout; break; }
		if (code == -1) { if (errno == EAGAIN) continue; }  // EAGAIN || EINTR ???
		if (code == -1) { status_ = stream_status_select ; error_ = errno; break;  }
		// reading data from file if they are there
		std::string::value_type buf;
		code = ::read(fd, &buf, sizeof(buf));
		if (code ==  0) { status_ = stream_status_eof; break; }
		if (code == -1) { if (errno == EAGAIN) continue; }
		if (code == -1) { status_ = stream_status_read; error_ = errno; break; }
		if (buf == terminator && !(skipbefore && result.empty())) break;
		if (buf != terminator) result += buf;
	}
	// setting timers
	if (timer)
	{
		gettimeofday(&time_now, NULL);
		timersub(&time_stop, &time_now, timer);
		if (timer->tv_sec < 0 || timer->tv_usec < 0) timerclear(timer);
	}
	// saving status and throwing exception if nowhere to save status
	if (status) *status = status_;
	if (error ) *error  = error_ ;
	else switch (status_)
	{
		case stream_status_nofd  : throw e_io("Non-existent fd "        + convert::fd2print(fd) + ".", error_, strerror(error_));
		case stream_status_select: throw e_io("Can not select from fd " + convert::fd2print(fd) + ".", error_, strerror(error_));
		case stream_status_read  : throw e_io("Can not read from fd "   + convert::fd2print(fd) + ".", error_, strerror(error_));
		default: NOP;
	}
	// returning result if not in strict mode or if no error
	return (strict && status_) ? std::string() : result;
}

t_ipc_val io::readval (t_fd fd, timeval * timer, t_stream_status * status, std::string::value_type terminator, bool strict, bool skipbefore)
{
	int error_;
	std::string s = _read(fd, timer, status, &error_, terminator, strict, skipbefore);
	return s;
}

t_ipc_vec io::readvec (t_fd fd, timeval * timer, t_stream_status * status, std::string::value_type terminator, bool strict, bool skipbefore, unsigned mincount, unsigned maxcount)
{
	t_stream_status status_ = stream_status_ok; int error_ = 0;
	t_ipc_vec result;
	while (!status_) {
		std::string line = _read(fd, timer, &status_, &error_, terminator, strict, skipbefore);
		skipbefore = false;
		if ( line.empty() && (!mincount || result.size() >= mincount)) break;
		if (!line.empty() || !strict || !status_)
		{
			result.push_back(line);
		}
		if (status_ || (maxcount && result.size() >= maxcount)) break;
	}
	// saveing status and throwing exception if nowhere to save status
	if (status) *status = status_;
	else switch (status_)
	{
		case stream_status_nofd  : throw e_io("Non-existent fd "        + convert::fd2print(fd) + ".", error_, strerror(error_));
		case stream_status_select: throw e_io("Can not select from fd " + convert::fd2print(fd) + ".", error_, strerror(error_));
		case stream_status_read  : throw e_io("Can not read from fd "   + convert::fd2print(fd) + ".", error_, strerror(error_));
		default: NOP;
	}
	// returning result if not in struct mode or no error
	return result;
}

t_ipc_map io::readmap (t_fd fd, timeval * timer, t_stream_status * status, std::string::value_type terminator, std::string::value_type assign, bool strict, bool skipbefore, unsigned mincount, unsigned maxcount)
{
	t_stream_status status_ = stream_status_ok; int error_ = 0;
	t_ipc_map result;
	while (!status_) {
		std::string line = _read(fd, timer, &status_, &error_, terminator, strict, skipbefore);
		skipbefore = false;
		if ( line.empty() && (!mincount || result.size() >= mincount)) break;
		if (!line.empty() || !strict || !status_)
		{
			std::string::size_type p = line.find(assign);
			if (p == std::string::npos) throw e_io("Can not parse map line '"+line+"'.");
			else result[line.substr(0,p)] = line.substr(p+1);
		}
		if (status_ || (maxcount && result.size() >= maxcount)) break;
	}
	// saveing status and throwing exception if nowhere to save status
	if (status) *status = status_;
	else switch (status_)
	{
		case stream_status_nofd  : throw e_io("Non-existent fd "        + convert::fd2print(fd) + ".", error_, strerror(error_));
		case stream_status_select: throw e_io("Can not select from fd " + convert::fd2print(fd) + ".", error_, strerror(error_));
		case stream_status_read  : throw e_io("Can not read from fd "   + convert::fd2print(fd) + ".", error_, strerror(error_));
		default: NOP;
	}
	// returning result if not in struct mode or no error
	return result;
}



void io::_write (t_fd fd, timeval * timer, t_stream_status * status, int * error, std::string::value_type terminator, std::string value)
{
	t_stream_status status_ = stream_status_ok; int error_ = 0;
	timeval time_start, time_stop, time_now, time_left;
	// prepairing timers
	if (timer)
	{
		if (timer->tv_sec < 0 || timer->tv_usec < 0 || (timer->tv_sec == 0 && timer->tv_usec == 0)) status_ = stream_status_timeout;
		gettimeofday(&time_start, NULL);
		timeradd(&time_start, timer, &time_stop);
	}
	// checking if fd exists
	if (!fd_ok(fd)) { status_ = stream_status_nofd; error_ = errno; }
	// writing
	std::string::size_type pos = 0; value += terminator;
	while (!status_)
	{
		int code;
		// calculating time left for io operations
		if (timer)
		{
			gettimeofday(&time_now, NULL);
			timersub(&time_stop, &time_now, &time_left);
			if (time_left.tv_sec < 0 || time_left.tv_usec < 0 || (time_left.tv_sec == 0 && time_left.tv_usec ==0))
				{ status_ = stream_status_timeout; break; }
		};
		// waiting for data from file during specified time
		fd_set fdset;
		FD_ZERO(&fdset);
		FD_SET(fd, &fdset);
		code = ::select(fd+1, NULL, &fdset, NULL, timer ? &time_left : NULL);
		if (code ==  0) { status_ = stream_status_timeout; break; }
		if (code == -1) { if (errno == EAGAIN) continue; }  // EAGAIN || EINTR ???
		if (code == -1) { status_ = stream_status_select ; error_ = errno; break;  }
		// writing data to file if it is ready
		std::string buf = value.substr(pos); int bufsize = buf.length();
		code = ::write(fd, buf.c_str(), bufsize);
		if (code ==  0) { status_ = stream_status_eof; break; }
		if (code == -1) { if (errno == EAGAIN) continue; }
		if (code == -1) { status_ = stream_status_write; error_ = errno; break; }
		if (code == bufsize) break;
		pos += code;
	}
	// setting timers
	if (timer)
	{
		gettimeofday(&time_now, NULL);
		timersub(&time_stop, &time_now, timer);
		if (timer->tv_sec < 0 || timer->tv_usec < 0) timerclear(timer);
	}
	// saveing status and throwing exception if nowhere to save status
	if (status) *status = status_;
	if (error ) *error  = error_ ;
	else switch (status_)
	{
		case stream_status_nofd  : throw e_io("Non-existent fd "      + convert::fd2print(fd) + ".", error_, strerror(error_));
		case stream_status_select: throw e_io("Can not select to fd " + convert::fd2print(fd) + ".", error_, strerror(error_));
		case stream_status_write : throw e_io("Can not write to fd "  + convert::fd2print(fd) + ".", error_, strerror(error_));
		default: NOP;
	}
}

void io::writeval (t_fd fd, timeval * timer, t_stream_status * status, std::string::value_type terminator, t_ipc_val value)
{
	int error_ = 0;
	_write(fd, timer, status, &error_, terminator, value);
}

void io::writevec (t_fd fd, timeval * timer, t_stream_status * status, std::string::value_type terminator, t_ipc_vec value)
{
	t_stream_status status_ = stream_status_ok; int error_ = 0;
	for (t_ipc_vec::const_iterator i = value.begin(); i != value.end(); i++)
	{
		_write(fd, timer, &status_, &error_, terminator, *i);
		if (status_) break;
	}
	if (!status_) _write(fd, timer, &status_, &error_, terminator, "");
	// saving status and throwing exception if nowhere to save status
	if (status) *status = status_;
	else switch (status_)
	{
		case stream_status_nofd  : throw e_io("Non-existent fd "      + convert::fd2print(fd) + ".", error_, strerror(error_));
		case stream_status_select: throw e_io("Can not select to fd " + convert::fd2print(fd) + ".", error_, strerror(error_));
		case stream_status_write : throw e_io("Can not write to fd "  + convert::fd2print(fd) + ".", error_, strerror(error_));
		default: NOP;
	}
}

void io::writemap (t_fd fd, timeval * timer, t_stream_status * status, std::string::value_type terminator, std::string::value_type assign, t_ipc_map value)
{
	t_stream_status status_ = stream_status_ok; int error_ = 0;
	for (t_ipc_map::const_iterator i = value.begin(); i != value.end(); i++)
	{
		_write(fd, timer, &status_, &error_, terminator, i->first + assign + i->second);
		if (status_) break;
	}
	if (!status_) _write(fd, timer, &status_, &error_, terminator, "");
	// saving status and throwing exception if nowhere to save status
	if (status) *status = status_;
	else switch (status_)
	{
		case stream_status_nofd  : throw e_io("Non-existent fd "      + convert::fd2print(fd) + ".", error_, strerror(error_));
		case stream_status_select: throw e_io("Can not select to fd " + convert::fd2print(fd) + ".", error_, strerror(error_));
		case stream_status_write : throw e_io("Can not write to fd "  + convert::fd2print(fd) + ".", error_, strerror(error_));
		default: NOP;
	}
}



void io::log (t_fd fd, std::string message)
{
//	message = convert::time2print(time(NULL), log_time_format) + " [" + convert::pident2print(getpid(), log_pid_length) + "] " + message;
	_write(fd, NULL, NULL, NULL, log_terminator, message);
}



bool io::fd_ok (t_fd fd)
{
	if (fd == -1) return false;
	int code = fcntl(fd, F_GETFL, 0);
	if (code != -1) return true;
//	if (errno == EBADF) return false;
	return false;
}

void io::fd_copy (t_fd oldfd, t_fd newfd)
{
	if (oldfd == newfd) return;
	if ((oldfd != -1) && (fcntl(oldfd, F_GETFL, 0) == -1))
		throw e_io("Can not copy fd "+convert::fd2print(oldfd)+" to fd "+convert::fd2print(newfd)+" because source fd is bad.", errno, strerror(errno));
	if (newfd != -1) close(newfd);
	if ((oldfd != -1) && (newfd != -1) && (fcntl(oldfd, F_DUPFD, newfd) == -1))
		throw e_io("Can not copy fd "+convert::fd2print(oldfd)+" to fd "+convert::fd2print(newfd)+" because duping failed.", errno, strerror(errno));
}

void io::fd_move (t_fd oldfd, t_fd newfd)
{
	fd_copy(oldfd, newfd);
	if (oldfd != -1) close(oldfd);
}

int io::fd_flags (t_fd fd)
{
	int result = fcntl(fd, F_GETFL, 0);
	if (result == -1) throw e_io("Can not get flags of fd "+convert::fd2print(fd)+".", errno, strerror(errno));
	return result;
}

void io::fd_flags (t_fd fd, int flags)
{
	int result = fcntl(fd, F_SETFL, flags);
	if (result == -1) throw e_io("Can not set flags of fd "+convert::fd2print(fd)+".", errno, strerror(errno));
}
