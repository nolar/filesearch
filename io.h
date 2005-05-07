#ifndef _FILESEARCH_IO_
#define _FILESEARCH_IO_

#include <string>
#include <vector>
#include <map>
#include "config.h"
#include "typedefs.h"
#include "e_io.h"

enum t_stream_status {
	stream_status_ok,
	stream_status_eof,
	stream_status_timeout,
	stream_status_nofd,
	stream_status_select,
	stream_status_read,
	stream_status_write
};

class io
{
private:
	static int _error;
protected:
	static std::string _time_pid_prefix ();
public:
	static t_fd fd_task;
	static t_fd fd_data;
	static t_fd fd_log;
	static t_fd fd_debug;
	static t_fd fd_null;
	static char * fd_null_path;

	static std::string::value_type log_terminator;
	static std::string log_time_format;
	static int         log_pid_length;

	// streaming routies
	static             std::string  readline  (t_fd fd, timeval * timer, t_stream_status * status, std::string::value_type terminator, bool strict, bool skipempty);
	static std::vector<std::string> readblock (t_fd fd, timeval * timer, t_stream_status * status, std::string::value_type terminator, bool strict, bool skipempty, unsigned mincount = 0, unsigned maxcount = 0);
	static                    void writeline  (t_fd fd, timeval * timer, t_stream_status * status, std::string::value_type terminator, std::string              value);
	static                    void writeblock (t_fd fd, timeval * timer, t_stream_status * status, std::string::value_type terminator, std::vector<std::string> value);
	static void log        (std::string              message);
	static void log        (std::vector<std::string> message);
	static void debug      (std::string              message);
	static void debug      (std::vector<std::string> message);

	// file descriptor handling
	static bool fd_ok (t_fd fd);
	static void fd_copy (t_fd oldfd, t_fd newfd);
	static void fd_move (t_fd oldfd, t_fd newfd);
	static int  fd_flags (t_fd fd);
	static void fd_flags (t_fd fd, int flags);
};

#endif
