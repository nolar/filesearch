#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/select.h>
#include "io.h"
#include "convert.h"



int io::_error;

t_fd io::fd_task  = 0;
t_fd io::fd_data  = 3;
t_fd io::fd_log   = 2;
t_fd io::fd_debug = 1;
t_fd io::fd_null  = -1;
char * io::fd_null_path = "/dev/null";

std::string::value_type io::log_terminator = '\n';
std::string io::log_time_format = "%Y-%m-%d %H:%M:%S";
int         io::log_pid_length  = 5;



std::string io::readline (t_fd fd, timeval * timer, t_stream_status * status, std::string::value_type terminator, bool strict, bool skipempty)
{
	t_stream_status status_ = stream_status_ok;
	timeval time_start, time_stop, time_now, time_left;
	// prepairing timers
	if (timer)
	{
		if (timer->tv_sec < 0 || timer->tv_usec < 0 || (timer->tv_sec == 0 && timer->tv_usec == 0)) { status_ = stream_status_timeout; }
		gettimeofday(&time_start, NULL);
		timeradd(&time_start, timer, &time_stop);
	}
	// checking if fd exists
	if (!fd_ok(fd)) { status_ = stream_status_nofd; } //!!! maybe make do{}while -> while(!status){} and not to return reesult here, but just to setup status to error, and add timeout checking before any while()ing
	// reading
	std::string result;
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
//!!!		Can not understand why select() does not work on pipes :-\ Assuming that fd is always ready for reading.
//!!!		code = (timer && ((time_left.tv_sec < 0) || (time_left.tv_usec < 0))) ? 0 : 1;
		code = ::select(fd+1, &fdset, NULL, NULL, timer ? &time_left : NULL);
		if (code ==  0) { status_ = stream_status_timeout; break; }
		if (code == -1) { if (errno == EAGAIN) continue; }  // EAGAIN || EINTR ???
		if (code == -1) { status_ = stream_status_select ; _error = errno; break;  }
		// reading data from file if they are there
		std::string::value_type buf;
		code = ::read(fd, &buf, sizeof(buf));
		if (code ==  0) { status_ = stream_status_eof; break; }
		if (code == -1) { if (errno == EAGAIN) continue; }
		if (code == -1) { status_ = stream_status_read; _error = errno;  break; }
		if (buf == terminator && !(skipempty && result.empty())) break;
		if (buf != terminator) result += buf;
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
	else switch (status_)
	{
		case stream_status_nofd  : throw e_io("Non-existent fd "        + convert::fd2print(fd) + ".", _error, strerror(_error));
		case stream_status_select: throw e_io("Can not select from fd " + convert::fd2print(fd) + ".", _error, strerror(_error));
		case stream_status_read  : throw e_io("Can not read from fd "   + convert::fd2print(fd) + ".", _error, strerror(_error));
		default: NOP;
	}
	// returning result if not in struct mode or no error
	return (strict && status_) ? std::string() : result;
}

std::vector<std::string> io::readblock (t_fd fd, timeval * timer, t_stream_status * status, std::string::value_type terminator, bool strict, bool skipempty, unsigned mincount, unsigned maxcount)
{
	t_stream_status status_ = stream_status_ok;
	std::vector<std::string> result;
	while (!status_) {
		std::string line = readline(fd, timer, &status_, terminator, strict, skipempty);
		skipempty = false;
		if ( line.empty() && (!mincount || result.size() >= mincount)) break;
		if (!line.empty() || !strict || !status_) result.push_back(line);
		if ( status_      || ( maxcount && result.size() >= maxcount)) break;
	}
	// saveing status and throwing exception if nowhere to save status
	if (status) *status = status_;
	else switch (status_)
	{
		case stream_status_nofd  : throw e_io("Non-existent fd "        + convert::fd2print(fd) + ".", _error, strerror(_error));
		case stream_status_select: throw e_io("Can not select from fd " + convert::fd2print(fd) + ".", _error, strerror(_error));
		case stream_status_read  : throw e_io("Can not read from fd "   + convert::fd2print(fd) + ".", _error, strerror(_error));
		default: NOP;
	}
	// returning result if not in struct mode or no error
	return result;
}

void io::writeline (t_fd fd, timeval * timer, t_stream_status * status, std::string::value_type terminator, std::string value)
{
	t_stream_status status_ = stream_status_ok;
	timeval time_start, time_stop, time_now, time_left;
	// prepairing timers
	if (timer)
	{
		if (timer->tv_sec < 0 || timer->tv_usec < 0 || (timer->tv_sec == 0 && timer->tv_usec == 0)) status_ = stream_status_timeout;
		gettimeofday(&time_start, NULL);
		timeradd(&time_start, timer, &time_stop);
	}
	// checking if fd exists
	if (!fd_ok(fd)) { status_ = stream_status_nofd; } //!!! maybe make do{}while -> while(!status){} and not to return reesult here, but just to setup status to error, and add timeout checking before any while()ing
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
//!!!		Can not understand why select() does not work for writing :-\ Assuming that fd is always ready for writing.
//!!!		code = timer ? ::select(1, NULL, &fdset, NULL, &time_left) : 1;
		code = ::select(fd+1, NULL, &fdset, NULL, timer ? &time_left : NULL);
//!!!		code = (timer && ((time_left.tv_sec < 0) || (time_left.tv_usec < 0))) ? 0 : 1;
		if (code ==  0) { status_ = stream_status_timeout; break; }
		if (code == -1) { if (errno == EAGAIN) continue; }  // EAGAIN || EINTR ???
		if (code == -1) { status_ = stream_status_select ; _error = errno; break;  }
		// writing data to file if it is ready
		std::string buf = value.substr(pos); int bufsize = buf.length();
		code = ::write(fd, buf.c_str(), bufsize);
		if (code ==  0) { status_ = stream_status_eof; break; }
		if (code == -1) { if (errno == EAGAIN) continue; }
		if (code == -1) { status_ = stream_status_write; _error = errno;  break; }
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
	else switch (status_)
	{
		case stream_status_nofd  : throw e_io("Non-existent fd "      + convert::fd2print(fd) + ".", _error, strerror(_error));
		case stream_status_select: throw e_io("Can not select to fd " + convert::fd2print(fd) + ".", _error, strerror(_error));
		case stream_status_write : throw e_io("Can not write to fd "  + convert::fd2print(fd) + ".", _error, strerror(_error));
		default: NOP;
	}
}

void io::writeblock (t_fd fd, timeval * timer, t_stream_status * status, std::string::value_type terminator, std::vector<std::string> value)
{
	t_stream_status status_ = stream_status_ok;
	value.push_back(std::string());
	for (std::vector<std::string>::const_iterator i = value.begin(); i != value.end(); i++)
	{
		writeline(fd, timer, &status_, terminator, *i);
		if (status_) break;
	}
	// saveing status and throwing exception if nowhere to save status
	if (status) *status = status_;
	else switch (status_)
	{
		case stream_status_nofd  : throw e_io("Non-existent fd "      + convert::fd2print(fd) + ".", _error, strerror(_error));
		case stream_status_select: throw e_io("Can not select to fd " + convert::fd2print(fd) + ".", _error, strerror(_error));
		case stream_status_write : throw e_io("Can not write to fd "  + convert::fd2print(fd) + ".", _error, strerror(_error));
		default: NOP;
	}
}



std::string io::_time_pid_prefix ()
{
	return convert::time2print(time(NULL), log_time_format) + " [" + convert::pident2print(getpid(), log_pid_length) + "] ";
}

void io::log (std::string message)
{
	message = _time_pid_prefix() + message;
	writeline(fd_log, NULL, NULL, log_terminator, message);
}

void io::log (std::vector<std::string> message)
{
	for (std::vector<std::string>::iterator i = message.begin(); i != message.end(); i++) *i = _time_pid_prefix() + *i;
	writeblock(fd_log, NULL, NULL, log_terminator, message);
}

void io::debug (std::string message)
{
	message = _time_pid_prefix() + message;
	writeline(fd_debug, NULL, NULL, log_terminator, message);
}

void io::debug (std::vector<std::string> message)
{
	for (std::vector<std::string>::iterator i = message.begin(); i != message.end(); i++) *i = _time_pid_prefix() + *i;
	writeblock(fd_debug, NULL, NULL, log_terminator, message);
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
