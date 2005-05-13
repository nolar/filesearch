#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/select.h>

#include "c_stream.h"
#include "c_object.h"
#include "c_stopsign.h"
#include "c_mapkey.h"
#include "c_signed.h"
#include "c_unsigned.h"
#include "c_double.h"
#include "c_flag.h"
#include "c_stamp.h"
#include "c_string.h"





c_stream::c_stream (t_fd fd, bool minimize_fd)
{
	f_fd = minimize_fd ? fd_minimize(fd) : fd;
	f_with_operation_timeout = false;
	f_with_global_timeout = false;
}

c_stream::c_stream (t_fd fd, long operation_timeout_sec, long operation_timeout_usec, bool minimize_fd)
{
	f_fd = minimize_fd ? fd_minimize(fd) : fd;
	f_with_operation_timeout = true;
	f_with_global_timeout = false;
	f_operation_timeout.tv_sec  = operation_timeout_sec ;
	f_operation_timeout.tv_usec = operation_timeout_usec;
}

c_stream::c_stream (t_fd fd, long operation_timeout_sec, long operation_timeout_usec, long global_timeout_sec, long global_timeout_usec, bool minimize_fd)
{
	f_fd = minimize_fd ? fd_minimize(fd) : fd;
	f_with_operation_timeout = true;
	f_with_global_timeout = true;
	f_operation_timeout.tv_sec  = operation_timeout_sec ;
	f_operation_timeout.tv_usec = operation_timeout_usec;
	f_global_timeout.tv_sec  = global_timeout_sec ;
	f_global_timeout.tv_usec = global_timeout_usec;
}

c_stream::~c_stream ()
{
}





c_stream::t_status c_stream::status ()
{
	return f_status;
}

c_stream::t_error c_stream::error ()
{
	return f_error;
}

c_stream::t_fd c_stream::fd ()
{
	return f_fd;
}

void c_stream::fd (c_stream::t_fd fd)
{
	f_fd = fd;
}





t_object_type c_stream::object_typeof (c_object * object)
{
	if (object == NULL) return ot_null; else
	if (dynamic_cast<c_stopsign*>(object)) return ot_stopsign; else
	if (dynamic_cast<c_mapkey  *>(object)) return ot_mapkey  ; else
	if (dynamic_cast<c_signed  *>(object)) return ot_signed  ; else
	if (dynamic_cast<c_unsigned*>(object)) return ot_unsigned; else
	if (dynamic_cast<c_double  *>(object)) return ot_double  ; else
	if (dynamic_cast<c_flag    *>(object)) return ot_flag    ; else
	if (dynamic_cast<c_stamp   *>(object)) return ot_stamp   ; else
	if (dynamic_cast<c_string  *>(object)) return ot_string  ; else
	return ot_null;
}

c_object * c_stream::object_create (t_object_type type)
{
	switch (type)
	{
		case ot_stopsign: return new c_stopsign;
		case ot_mapkey  : return new c_mapkey  ;
		case ot_signed  : return new c_signed  ;
		case ot_unsigned: return new c_unsigned;
		case ot_double  : return new c_double  ;
		case ot_flag    : return new c_flag    ;
		case ot_stamp   : return new c_stamp   ;
		case ot_string  : return new c_string  ;
		default: return NULL;
	}
}

const char * c_stream::object_name (t_object_type type)
{
	switch (type)
	{
		case ot_stopsign: return "stopsign";
		case ot_mapkey  : return "mapkey"  ;
		case ot_signed  : return "signed"  ;
		case ot_unsigned: return "unsigned";
		case ot_double  : return "double"  ;
		case ot_flag    : return "flag"    ;
		case ot_stamp   : return "stamp"   ;
		case ot_string  : return "string"  ;
		default: return "null(unknown)";
	}
}

const char * c_stream::object_name (c_object * object)
{
	return object_name(object_typeof(object));
}





c_object * c_stream::read_object ()
{
	t_object_type type;
	_read(&type, sizeof(type));
	if (status()) return NULL;

	t_object_size size;
	_read(&size, sizeof(size));
	if (status()) return NULL;

	char * buffer = NULL;
	if (size)
	{
		buffer = new char[size];
		_read(buffer, size);
		if (status()) return NULL;
	}

	c_object * object = object_create(type);
	if (object) object->stream_setdata(buffer, size);

	delete[] buffer;
	return object;
}

std::vector<c_object*> c_stream::read_vector ()
{
	std::vector<c_object*> result;
	do {
		c_object * object = read_object();
		if (dynamic_cast<c_stopsign*>(object)) { delete object; break; } else
		{
			result.push_back(object);
		}
	} while (true);
	return result;
}

std::map<c_mapkey,c_object*> c_stream::read_map ()
{
	std::map<c_mapkey,c_object*> result;
	c_mapkey * currentkey = NULL;
	do {
		c_object * object = read_object();
		if (dynamic_cast<c_stopsign*>(object)) { delete object; break; } else
		if (dynamic_cast<c_mapkey  *>(object)) { if (currentkey) delete currentkey; currentkey = dynamic_cast<c_mapkey*>(object); } else
		{
			if (currentkey)
			{
				result[*currentkey] = object;
				delete currentkey;
				currentkey = NULL;
			} else {
				delete object;
			}
		}
	} while (true);
	if (currentkey) delete currentkey;
	return result;
}

//std::map<t_key,c_object*> read_map    (t_object_type required_type)
//std::map<t_key,c_object*> read_map    (std::map<t_mapkey,t_object_type> required_types)





void c_stream::write_object (c_object * object)
{
	t_object_type type = object_typeof(object);
	_write(&type, sizeof(type));
	if (status()) return; //throw???

	t_object_size size = object->stream_size();
	_write(&size, sizeof(size));
	if (status()) return; //throw???

	char * buffer = NULL;
	if (size)
	{
		buffer = new char[size];
		object->stream_getdata(buffer, size);
		_write(buffer, size);
		if (status()) return; //??? throw?
	}

	delete[] buffer;
}

void c_stream::write_vector (std::vector<c_object*> objects)
{
	for (std::vector<c_object*>::const_iterator i = objects.begin(); i != objects.end(); i++)
	{
		write_object(*i);
	}
	c_stopsign ss;
	write_object(&ss);
}

void c_stream::write_map (std::map<c_mapkey,c_object*> objects)
{
	for (std::map<c_mapkey,c_object*>::const_iterator i = objects.begin(); i != objects.end(); i++)
	{
		write_object(const_cast<c_mapkey*>(&(i->first )));
		write_object(const_cast<c_object*>( (i->second)));
	}
	c_stopsign ss;
	write_object(&ss);
}












//t_fd c_stream::fd_task  = 0;
//t_fd c_stream::fd_data  = 3;
//t_fd c_stream::fd_log   = 2;
//t_fd c_stream::fd_debug = 1;
//t_fd c_stream::fd_null  = -1;
//char * c_stream::fd_null_file = "/dev/null";

//std::string::value_type c_stream::log_terminator  = '\n';
//std::string             c_stream::log_time_format = "%Y-%m-%d %H:%M:%S";
//int                     c_stream::log_pid_length  = 5;



void c_stream::_read (void * buffer, t_object_size size)
{
	f_status = st_ok; f_error = 0;
//	timeval time_start, time_stop, time_now, time_left;
	// prepairing timers
	if (have_op_timer)
	{
		if (timercmp(f_op_time_used, f_op_timeout, >=)) { f_status = st_op_timeout; }
	}
	if (have_io_timer)
	{
		if (timercmp(f_io_time_used, f_io_timeout, >=)) { f_status = st_io_timeout; }
	}
	if ((have_op_timer || have_io_timer) && !f_status)
	{
		gettimeofday(&time_start, NULL);
	}
	// checking if fd exists
//	if (!fd_ok(fd)) { status_ = stream_status_nofd; error_ = errno; }
	// reading
	int count = 0;
	while (!f_status)
	{
		int code;
		// calculating time left for io operation
		if (have_op_timer || have_io_timer)
		{
			gettimeofday(&time_now, NULL);
		}
		if (have_op_timer)
		{
			
		}
//		if (timer)
//		{
//			timersub(&time_stop, &time_now, &time_left);
//			if (time_left.tv_sec < 0 || time_left.tv_usec < 0 || (time_left.tv_sec == 0 && time_left.tv_usec ==0))
//				{ status_ = stream_status_timeout; break; }
//		}
		// waiting for data from file during specified time
		fd_set fdset;
		FD_ZERO(&fdset);
		FD_SET(f_fd, &fdset);
//		code = ::select(f_fd+1, &fdset, NULL, NULL, timer ? &time_left : NULL);
		code = ::select(f_fd+1, &fdset, NULL, NULL, NULL);
		if (code ==  0) { f_status = st_timeout_operation; break; }
		if (code == -1) { if (errno == EAGAIN) continue; }  // EAGAIN || EINTR ???
		if (code == -1) { f_status = st_error_select; f_error = errno; break;  }
		// reading data from file if they are there
		code = ::read(f_fd, static_cast<char*>(buffer)+count, sizeof(char));
		if (code ==  0) { f_status = st_eof; break; }
		if (code == -1) { if (errno == EAGAIN) continue; }
		if (code == -1) { f_status = st_error_read; f_error = errno; break; }
		count += code;
		if (count >= size) break;
	}
	// setting timers
//	if (timer)
//	{
//		gettimeofday(&time_now, NULL);
//		timersub(&time_stop, &time_now, timer);
//		if (timer->tv_sec < 0 || timer->tv_usec < 0) timerclear(timer);
//	}
	// saving status and throwing exception if nowhere to save status
//	switch (f_status)
//	{
//		case stream_status_nofd  : throw e_io("Non-existent fd "        + convert::fd2print(fd) + ".", error_, strerror(error_));
//		case stream_status_select: throw e_io("Can not select from fd " + convert::fd2print(fd) + ".", error_, strerror(error_));
//		case stream_status_read  : throw e_io("Can not read from fd "   + convert::fd2print(fd) + ".", error_, strerror(error_));
//		default: NOP;
//	}
	// returning result if not in strict mode or if no error
//	return (strict && status_) ? std::string() : result;
}

void c_stream::_write (void * buffer, t_object_size size)
{
	f_status = st_ok;
	::write(f_fd, buffer, size);
}






c_stream::t_fd c_stream::fd_minimize (c_stream::t_fd fd)
{
	return fd; //TODO!!!
}
