#ifndef _FILESEARCH_STREAM_
#define _FILESEARCH_STREAM_

#include <vector>
#include <map>
#include <time.h>

#include "c_object.h"
#include "c_mapkey.h"

enum t_object_type {
	ot_null, ot_stopsign, ot_mapkey,
	ot_signed, ot_unsigned, ot_double, ot_flag, ot_stamp, ot_string };

class c_stream
{
public:
	typedef int t_fd;
	typedef int t_error;
	typedef enum {	st_ok, st_eof,
			st_timeout_operation, st_timeout_global,
			st_error_poll, st_error_select, st_error_read, st_error_write
		} t_status;
private:
	t_fd f_fd;
	t_status f_status;
	t_error  f_error ;

	bool have_op_timeout;
	bool have_io_timeout;
	timeval f_op_timeout, f_op_timeleft;
	timeval f_io_timeout, f_op_timeleft;
protected:
	void _read  (void * buffer, t_object_size size);
	void _write (void * buffer, t_object_size size);
public:
	// typedefs

	// constructors and destructor
	c_stream (t_fd fd, bool minimize_fd = false);
	c_stream (t_fd fd, long op_timeout_sec, long op_timeout_usec, bool minimize_fd = false);
	c_stream (t_fd fd, long op_timeout_sec, long op_timeout_usec, long io_timeout_sec, long global_timeout_usec, bool minimize_fd = false);
	virtual ~c_stream ();

	//
	t_status status ();
	int      error  ();
	t_fd fd ();
	void fd (t_fd fd);
	timeval op_time_left ();
	timeval io_time_left ();
	void    set_op_timeout (timeval timeout);
	void    set_op_timeout (long sec, long usec);
	void    set_io_timeout (timeval timeout);
	void    set_io_timeout (long sec, long usec);
	void    reset_op_timer ();
	void    reset_io_timer ();

	//
	static t_object_type object_typeof (c_object * object);
	static c_object *   object_create (t_object_type type);
	static const char * object_name   (t_object_type type);
	static const char * object_name   (c_object * object);

	//
	               c_object*  read_object ();
//	               c_object*  read_object (t_object_type required_type);
	std::vector   <c_object*> read_vector ();
//	std::vector   <c_object*> read_vector (t_object_type stop_type, t_object_type required_type);
//	std::vector   <c_object*> read_vector (t_object_type stop_type, std::vector<t_object_type> required_types);
	std::map<c_mapkey,c_object*> read_map    ();
//	std::map<t_key,c_object*> read_map    (t_object_type required_type);
//	std::map<t_key,c_object*> read_map    (std::map<t_mapkey,t_object_type> required_types);

	//
	void write_object (c_object * object);
	void write_vector (std::vector<c_object*> objects);
	void write_map    (std::map<c_mapkey,c_object*> objects);

	//
	static t_fd fd_minimize (t_fd fd);
};

#endif
