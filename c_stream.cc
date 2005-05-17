#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/select.h>
#include <memory>

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
#include "c_ipaddr.h"
#include "c_path.h"
//!!! DEBUG
#include <iostream>
#include <iomanip>
using namespace std;





c_stream s_log  ;
c_stream s_debug;
std::string c_stream::_log_time_format = "%Y-%m-%d %H:%M:%S";
unsigned    c_stream::_log_pid_size = 5;




c_stream::c_stream ()
{
	f_fd_defined = false;
	f_fd = -1;
	f_have_min_timeout = f_have_max_timeout = false;
	f_status = st_ok; f_error = 0;
}

c_stream::c_stream (t_fd fd, bool minimize_fd)
{
	f_fd_defined = true;
	f_fd = minimize_fd ? fd_minimize(fd) : fd;
	f_have_min_timeout = f_have_max_timeout = false;
	f_status = st_ok; f_error = 0;
}

c_stream::~c_stream ()
{
}





c_stream::t_fd c_stream::fd ()
{
	return f_fd;
}

void c_stream::fd (c_stream::t_fd fd)
{
	f_fd = fd;
	// check if fd is ok with fcntl()
	f_fd_defined = true;
}

c_stream::t_status c_stream::status ()
{
	return f_fd_defined ? f_status : st_nofd;
}

c_stream::t_error c_stream::error ()
{
	return f_error;
}

void c_stream::set_min_timeout (timeval value)
{
	if (value.tv_sec < 0 || value.tv_usec < 0) unset_min_timeout(); else
	if (f_have_max_timeout && timercmp(&f_max_timeout, &value, <)) unset_min_timeout(); else
	{
		f_have_min_timeout = true;
		f_min_timeout = value;
	}
}

void c_stream::set_max_timeout (timeval value)
{
	if (value.tv_sec < 0 || value.tv_usec < 0) unset_max_timeout(); else
	if (f_have_min_timeout && timercmp(&f_min_timeout, &value, >)) unset_max_timeout(); else
	{
		f_have_max_timeout = true;
		f_max_timeout = value;
	}
}

void c_stream::set_min_timeout (long sec, long usec)
{
	timeval t; t.tv_sec = sec; t.tv_usec = usec;
	set_min_timeout(t);
}

void c_stream::set_max_timeout (long sec, long usec)
{
	timeval t; t.tv_sec = sec; t.tv_usec = usec;
	set_max_timeout(t);
}

void c_stream::unset_min_timeout ()
{
	f_have_min_timeout = false;
}

void c_stream::unset_max_timeout ()
{
	f_have_max_timeout = false;
}

bool c_stream::have_min_timeout ()
{
	return f_have_min_timeout;
}

bool c_stream::have_max_timeout ()
{
	return f_have_max_timeout;
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
	if (dynamic_cast<c_ipaddr  *>(object)) return ot_ipaddr  ; else
	if (dynamic_cast<c_path    *>(object)) return ot_path    ; else
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
		case ot_ipaddr  : return new c_ipaddr  ;
		case ot_path    : return new c_path    ;
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
		case ot_ipaddr  : return "ipaddr"  ;
		case ot_path    : return "path"    ;
		default: return "null(unknown)";
	}
}

const char * c_stream::object_name (c_object * object)
{
	return object_name(object_typeof(object));
}





c_object * c_stream::read_object (timeval * timer)
{
	cerr << "Ring T" << endl;
	t_object_type type;
	_read(&type, sizeof(type), timer);
	cerr << "Red  T=" << type<<"("<<object_name(type)<<") st="<<status()<< endl;
	if (status()) return NULL;

	t_object_size size;
	_read(&size, sizeof(size), timer);
	if (status()) return NULL;

	std::auto_ptr<char> buffer (size ? new char[size] : NULL);
	if (size)
	{
		_read(buffer.get(), size, timer);
		if (status()) return NULL;
	}

	c_object * object = object_create(type);
	if (object) object->stream_setdata(buffer.get(), size);
	return object;
}

c_stream::t_vector c_stream::read_vector (timeval * timer)
{
	std::vector<c_object*> result;
	do {
		c_object * object = read_object(timer);
		if (f_status) break;
		if (dynamic_cast<c_stopsign*>(object)) { delete object; break; } else
		{
			result.push_back(object);
		}
	} while (true);
	return result;
}

c_stream::t_map c_stream::read_map (timeval * timer)
{
	std::map<c_mapkey,c_object*> result;
	c_mapkey * currentkey = NULL;
	do {
		c_object * object = read_object(timer);
		if (f_status) break;
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





void c_stream::write_object (c_object * object, timeval * timer)
{
	t_object_type type = object_typeof(object);
	_write(&type, sizeof(type), timer);
	if (status()) return; //throw???

	t_object_size size = object->stream_size();
	_write(&size, sizeof(size), timer);
	if (status()) return; //throw???

	std::auto_ptr<char> buffer(size ? new char[size] : NULL);
	if (size)
	{
		object->stream_getdata(buffer.get(), size);
		_write(buffer.get(), size, timer);
		if (status()) return; //??? throw?
	}
}

void c_stream::write_vector (c_stream::t_vector objects, timeval * timer)
{
	for (std::vector<c_object*>::const_iterator i = objects.begin(); i != objects.end(); i++)
	{
		write_object(*i);
		if (f_status) break;
	}
	c_stopsign ss;
	write_object(&ss);
}

void c_stream::write_map (c_stream::t_map objects, timeval * timer)
{
	for (std::map<c_mapkey,c_object*>::const_iterator i = objects.begin(); i != objects.end(); i++)
	{
		write_object(const_cast<c_mapkey*>(&(i->first )));
		if (f_status) break;
		write_object(const_cast<c_object*>( (i->second)));
		if (f_status) break;
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



bool c_stream::_timing_start (timeval * timer, timeval * deadline)
{
	if (!deadline) throw 1; //!!!
	timeval timeout, startup;
	if (timer)
	{
		if (f_have_max_timeout && timercmp(timer, &f_max_timeout, >)) timeout = f_max_timeout; else
		if (f_have_min_timeout && timercmp(timer, &f_min_timeout, <)) timeout = f_min_timeout; else
		timeout = *timer;
	} else {
		if (f_have_max_timeout) timeout = f_max_timeout; else
		if (f_have_min_timeout) timeout = f_min_timeout; else
		timerclear(&timeout);
	}
	if (timeout.tv_sec < 0 || timeout.tv_usec < 0 || (timeout.tv_sec == 0 && timeout.tv_usec == 0))
	{
		timerclear(deadline);
		return true;
	} else {
		gettimeofday(&startup, NULL);
		timeradd(&startup, &timeout, deadline);
		return false;
	}
}

bool c_stream::_timing_cycle (timeval * deadline, timeval * timeout)
{
	if (!deadline) throw 1; //!!!
	if (!timeout ) throw 1; //!!!
	timeval now;
	gettimeofday(&now, NULL);
	timersub(deadline, &now, timeout);
	return (timeout->tv_sec < 0 || timeout->tv_usec < 0 || (timeout->tv_sec == 0 && timeout->tv_usec == 0));
}

bool c_stream::_timing_finish (timeval * timer, timeval * deadline)
{
	if (!deadline) throw 1; //!!!
	if (timer)
	{
		timeval now;
		gettimeofday(&now, NULL);
		timersub(deadline, &now, timer);
		if (timer->tv_sec < 0 || timer->tv_usec < 0) timerclear(timer);
		return (timer->tv_sec == 0 && timer->tv_usec == 0);
	} else {
		return false;
	}
}





void c_stream::_read (void * buffer, t_object_size size, timeval * timer)
{
	// prepairing timers
	timeval deadline;
	if (_timing_start(timer, &deadline)) { f_status = st_timeout; }
	// reading
	f_status = st_ok; f_error = 0;
	if (!f_fd_defined) f_status = st_nofd;
	t_object_size count = 0;
	while (!f_status)
	{
		int code;
		// calculating time left for io operation
		timeval timeout;
		if (_timing_cycle(&deadline, &timeout)) { f_status = st_timeout; break; }
		// waiting for data from file during specified time
		fd_set fdset;
		FD_ZERO(&fdset);
		FD_SET(f_fd, &fdset);
		code = ::select(f_fd+1, &fdset, NULL, NULL, &timeout);
		if (code ==  0) { f_status = st_timeout; break; }
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
	if (_timing_finish(timer, &deadline)) { f_status = st_timeout; }
}

void c_stream::_write (void * buffer, t_object_size size, timeval * timer)
{
	// prepairing timers
	timeval deadline;
	if (_timing_start(timer, &deadline)) { f_status = st_timeout; }
	// writing
	f_status = st_ok; f_error = 0;
	if (!f_fd_defined) f_status = st_nofd;
	t_object_size count = 0;
	while (!f_status)
	{
		int code;
		// calculating time left for io operations
		timeval timeout;
		if (_timing_cycle(&deadline, &timeout)) { f_status = st_timeout; break; }
		// waiting for data from file during specified time
		fd_set fdset;
		FD_ZERO(&fdset);
		FD_SET(f_fd, &fdset);
		code = ::select(f_fd+1, NULL, &fdset, NULL, &timeout);
		if (code ==  0) { f_status = st_timeout; break; }
		if (code == -1) { if (errno == EAGAIN) continue; }  // EAGAIN || EINTR ???
		if (code == -1) { f_status = st_error_select ; f_error = errno; break;  }
		// writing data to file if it is ready
		code = ::write(f_fd, static_cast<char*>(buffer)+count, size-count);
		if (code ==  0) { f_status = st_eof; break; }
		if (code == -1) { if (errno == EAGAIN) continue; }
		if (code == -1) { f_status = st_error_write; f_error = errno; break; }
		count += code;
		if (count >= size) break;
	}
	// setting timers
	if (_timing_finish(timer, &deadline)) { f_status = st_timeout; }
}






void c_stream::stamped (std::string message)
{
	c_stamp t(::time(NULL));
	c_unsigned p(::getpid());
	return t.ascii(_log_time_format) + " [" + p.ascii(_log_pid_size) + "] " + message;
}





c_stream::t_fd c_stream::fd_minimize (c_stream::t_fd fd)
{
	return fd; //TODO!!!
}
