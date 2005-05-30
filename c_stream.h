#ifndef _FILESEARCH_STREAM_
#define _FILESEARCH_STREAM_

#include <vector>
#include <map>
#include <time.h>

#include "c_object.h"
#include "c_mapkey.h"


class c_stream
{
public:
	typedef int t_fd;
	typedef int t_error;
	typedef enum {	st_ok, st_eof, st_timeout, st_nofd,
			st_error_poll, st_error_select, st_error_read, st_error_write
		} t_status;
	typedef std::vector<c_object*> t_vector;
	typedef std::map<c_mapkey,c_object*> t_map;
	typedef unsigned char t_object_type;
	static const t_object_type ot_null;
	static const t_object_type ot_stopsign;
	static const t_object_type ot_mapkey;
	static const t_object_type ot_signed;
	static const t_object_type ot_unsigned;
	static const t_object_type ot_double;
	static const t_object_type ot_flag;
	static const t_object_type ot_stamp;
	static const t_object_type ot_string;
	static const t_object_type ot_ipaddr;
	static const t_object_type ot_path;
	static const t_object_type ot_action;
	static const t_object_type ot_protocol;
private:
	bool     f_fd_defined;
	t_fd     f_fd;
	t_status f_status;
	t_error  f_error ;
	bool f_have_min_timeout; timeval f_min_timeout;
	bool f_have_max_timeout; timeval f_max_timeout;
protected:
	bool _timing_start  (timeval * timer, timeval * deadline);
	bool _timing_cycle  (timeval * deadline, timeval * timeout);
	bool _timing_finish (timeval * timer, timeval * deadline);
	void _read  (      void * buffer, t_object_size size, timeval * timer = NULL);
	void _write (const void * buffer, t_object_size size, timeval * timer = NULL);
public:
	// constructors and destructor
	c_stream ();
	c_stream (t_fd fd, bool minimize_fd = false);
	virtual ~c_stream ();

	//
	t_fd fd () const;
	void fd (t_fd fd);
	t_status status () const;
	t_error  error  () const;
	const char * status_text () const;
	void set_min_timeout (timeval value);
	void set_max_timeout (timeval value);
	void set_min_timeout (long sec, long usec = 0);
	void set_max_timeout (long sec, long usec = 0);
	void unset_min_timeout ();
	void unset_max_timeout ();
	bool have_min_timeout () const;
	bool have_max_timeout () const;

	//
	static t_object_type object_typeof (c_object * object);
	static c_object *   object_create (t_object_type type);
	static const char * object_name   (t_object_type type);
	static const char * object_name   (c_object * object);

	//
	c_object * read_object (timeval * timer = NULL);
	t_vector   read_vector (timeval * timer = NULL);
	t_map      read_map    (timeval * timer = NULL);

	//
	void write_object (c_object * object , timeval * timer = NULL);
	void write_vector (t_vector   objects, timeval * timer = NULL);
	void write_map    (t_map      objects, timeval * timer = NULL);

	//
	void stamped (std::string message);

	//
	static t_fd fd_minimize (t_fd fd);
};

extern c_stream s_log;
extern c_stream s_debug;

#endif
