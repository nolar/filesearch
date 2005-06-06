#ifndef _FILESEARCH_STREAM_
#define _FILESEARCH_STREAM_

#include "config.h"
#include "c_streambox.h"
#include "e_stream.h"
#include "e_stream_eof.h"
#include "e_stream_timeout.h"

#include <vector>
#include <map>
#include <time.h>

class c_stream
{
public:
	typedef int t_fd;
	typedef std::vector<c_streambox> t_vector;
	typedef std::map<c_identifier,c_streambox> t_map;
private:
	bool     f_fd_defined;
	t_fd     f_fd;
	bool f_have_min_timeout; timeval f_min_timeout;
	bool f_have_max_timeout; timeval f_max_timeout;
protected:
	bool _timing_start  (timeval * timer, timeval * deadline);
	bool _timing_cycle  (timeval * deadline, timeval * timeout);
	bool _timing_finish (timeval * timer, timeval * deadline);
	void _read  (      void * buffer, t_object_size size, timeval * timer = NULL);
	void _write (const void * buffer, t_object_size size, timeval * timer = NULL);
	static t_fd fd_minimize (t_fd fd);
public:
	// constructors and destructor
	c_stream ();
	c_stream (t_fd fd, bool minimize_fd = false);
	virtual ~c_stream ();
	// value handlnig routines
	t_fd get_fd () const;
	void set_fd (t_fd fd);
	void set_min_timeout (timeval value);
	void set_max_timeout (timeval value);
	void set_min_timeout (long sec, long usec = 0);
	void set_max_timeout (long sec, long usec = 0);
	void unset_min_timeout ();
	void unset_max_timeout ();
	bool have_min_timeout () const;
	bool have_max_timeout () const;
	// reading routines
	c_streambox read_object (timeval * timer = NULL);
	t_vector    read_vector (timeval * timer = NULL);
	t_map       read_map    (timeval * timer = NULL);
	// writing routines
	void write_object (c_streambox object, timeval * timer = NULL);
	void write_vector (t_vector   objects, timeval * timer = NULL);
	void write_map    (t_map      objects, timeval * timer = NULL);
	// logging routines
	void stamped (std::string message);
};

extern c_stream s_log;
extern c_stream s_debug;

#endif
