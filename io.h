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
protected:
public:
	// file descriptors
	// warning: if you want something to be ignored, open() it to io::fd_null_file before.
	static t_fd fd_ipc_task;
	static t_fd fd_ipc_data;
	static t_fd fd_log;
	static t_fd fd_debug;
	static t_fd fd_null;
	static char * fd_null_file;

	// format specifiers for logging
	static std::string::value_type log_terminator ;
	static std::string             log_time_format;
	static int                     log_pid_length ;

	// streaming routies

	
	static std::string _read   (t_fd fd, timeval * timer, t_stream_status * status, int * error, std::string::value_type terminator, bool strict, bool skipbefore);
	static t_ipc_val   readval (t_fd fd, timeval * timer, t_stream_status * status, std::string::value_type terminator,                                 bool strict, bool skipbefore);
	static t_ipc_vec   readvec (t_fd fd, timeval * timer, t_stream_status * status, std::string::value_type terminator,                                 bool strict, bool skipbefore, unsigned mincount = 0, unsigned maxcount = 0);
	static t_ipc_map   readmap (t_fd fd, timeval * timer, t_stream_status * status, std::string::value_type terminator, std::string::value_type assign, bool strict, bool skipbefore, unsigned mincount = 0, unsigned maxcount = 0);

	static void       _write   (t_fd fd, timeval * timer, t_stream_status * status, int * error, std::string::value_type terminator, std::string value);
	static void       writeval (t_fd fd, timeval * timer, t_stream_status * status, std::string::value_type terminator,                                 t_ipc_val   value);
	static void       writevec (t_fd fd, timeval * timer, t_stream_status * status, std::string::value_type terminator,                                 t_ipc_vec   value);
	static void       writemap (t_fd fd, timeval * timer, t_stream_status * status, std::string::value_type terminator, std::string::value_type assign, t_ipc_map   value);

	static void            log (t_fd fd, std::string message);

	// file descriptor handling
	static bool fd_ok (t_fd fd);
	static void fd_copy (t_fd oldfd, t_fd newfd);
	static void fd_move (t_fd oldfd, t_fd newfd);
	static int  fd_flags (t_fd fd);
	static void fd_flags (t_fd fd, int flags);
};

#endif
