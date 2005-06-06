#include "c_stream.h"
#include "globals.h"
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/select.h>
#include <memory>





c_stream s_log  ;
c_stream s_debug;





c_stream::c_stream ()
{
	f_fd_defined = false;
	f_fd = -1;
	f_have_min_timeout = f_have_max_timeout = false;
}

c_stream::c_stream (t_fd fd, bool minimize_fd)
{
	f_fd_defined = true;
	f_fd = minimize_fd ? fd_minimize(fd) : fd;
	f_have_min_timeout = f_have_max_timeout = false;
}

c_stream::~c_stream ()
{
}





c_stream::t_fd c_stream::get_fd () const
{
	return f_fd;
}

void c_stream::set_fd (c_stream::t_fd fd)
{
	f_fd = fd;
	// check if fd is ok with fcntl()
	f_fd_defined = true;
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

bool c_stream::have_min_timeout () const
{
	return f_have_min_timeout;
}

bool c_stream::have_max_timeout () const
{
	return f_have_max_timeout;
}





c_streambox c_stream::read_object (timeval * timer)
{
	c_streambox::t_object_type type;
	_read(&type, sizeof(type), timer);

	c_streambox box(type);
	if (!box.get()) throw e_stream(__FILE__,__LINE__,std::string()+"Can not create object of type '"+box.name()+"' while reading.");

	t_object_size size;
	if (box.get()->stream_vary())
	{
		_read(&size, sizeof(size), timer);
	} else {
		size = box.get()->stream_size();
	}

	if (size)
	{
		std::auto_ptr<char> buffer(new char[size]);
		_read(buffer.get(), size, timer);
		box.get()->stream_setdata(buffer.get(), size);
	}

	return box;
}

c_stream::t_vector c_stream::read_vector (timeval * timer)
{
	t_vector result;
	do {
		c_streambox box = c_streambox(read_object(timer));
		if (box.is_stopsign()) break;
		else result.push_back(box);
	} while (true);
	return result;
}

c_stream::t_map c_stream::read_map (timeval * timer)
{
	t_map result;
	c_identifier currentkey; bool havekey = false;
	do {
		c_streambox box = c_streambox(read_object(timer));
		if (box.is_stopsign()) break; else
		if (box.is_identifier()) { havekey = true; currentkey = box.as_identifier(); } else
		{
			if (havekey)
			{
				result[currentkey] = box;
				havekey = false;
			} else throw e_stream(__FILE__,__LINE__,std::string()+"Wrong format of map in stream - object of type '"+box.name()+"' without preceding identifier.");
		}
	} while (true);
	return result;
}





void c_stream::write_object (c_streambox streambox, timeval * timer)
{
	//!!! check for NULL object here and exit if it is

	c_streambox::t_object_type type = streambox.type();
	_write(&type, sizeof(type), timer);

	t_object_size size = streambox.get()->stream_size();
	if (streambox.get()->stream_vary())
	{
		_write(&size, sizeof(size), timer);
	}

	if (size)
	{
		std::auto_ptr<char> buffer(new char[size]);
		streambox.get()->stream_getdata(buffer.get(), size);
		_write(buffer.get(), size, timer);
	}
}

void c_stream::write_vector (c_stream::t_vector objects, timeval * timer)
{
	for (t_vector::const_iterator i = objects.begin(); i != objects.end(); i++)
	{
		write_object(*i, timer);
	}
	write_object(c_streambox(new c_stopsign()), timer);
}

void c_stream::write_map (c_stream::t_map objects, timeval * timer)
{
	for (t_map::const_iterator i = objects.begin(); i != objects.end(); i++)
	{
		write_object(c_streambox(new c_identifier(i->first)), timer);
		write_object(i->second, timer);
	}
	write_object(c_streambox(new c_stopsign()), timer);
}





void c_stream::stamped (std::string message)
{
	timeval timer;
	timer.tv_sec  = logging_timeout__sec ;
	timer.tv_usec = logging_timeout__usec;
	c_stamp t(::time(NULL));
	c_unsigned p(::getpid());
	std::string s = t.ascii(logging_time_format) + " [" + p.ascii(logging_pid_length) + "] " + message + logging_terminator;
	try { _write(static_cast<const char*>(s.c_str()), s.length(), &timer); }
	catch (e_stream & e) {}
}





bool c_stream::_timing_start (timeval * timer, timeval * deadline)
{
	if (!deadline) throw e_stream(__FILE__,__LINE__,"Trying to measure start time without deadline pointer.");
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
	if (!deadline) throw e_stream(__FILE__,__LINE__,"Trying to measure start time without deadline pointer.");
	if (!timeout ) throw e_stream(__FILE__,__LINE__,"Trying to measure start time without timeout pointer.");
	timeval now;
	gettimeofday(&now, NULL);
	timersub(deadline, &now, timeout);
	return (timeout->tv_sec < 0 || timeout->tv_usec < 0 || (timeout->tv_sec == 0 && timeout->tv_usec == 0));
}

bool c_stream::_timing_finish (timeval * timer, timeval * deadline)
{
	if (!deadline) throw e_stream(__FILE__,__LINE__,"Trying to measure start time without deadline pointer.");
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
	if (_timing_start(timer, &deadline)) throw e_stream_timeout(__FILE__,__LINE__,"Timeout before read operation (precondition).");
	// reading
	if (!f_fd_defined) throw e_stream(__FILE__,__LINE__,std::string()+"No fd defined in stream.");
	t_object_size count = 0;
	while (true)
	{
		int code;
		// calculating time left for io operation
		timeval timeout;
		if (_timing_cycle(&deadline, &timeout)) throw e_stream_timeout(__FILE__,__LINE__,"Timeout in read operation (in cycle).");
		// waiting for data from file during specified time
		fd_set fdset;
		FD_ZERO(&fdset);
		FD_SET(f_fd, &fdset);
		code = ::select(f_fd+1, &fdset, NULL, NULL, &timeout);
		if (code ==  0) throw e_stream_timeout(__FILE__,__LINE__,"Timeout when selecting for read.");
		if (code == -1) { if (errno == EAGAIN) continue; } // EAGAIN || EINTR ???
		if (code == -1) throw e_stream(__FILE__,__LINE__,std::string()+"Error while select()ing fd for read().", errno);
		// reading data from file if they are there
		code = ::read(f_fd, static_cast<char*>(buffer)+count, sizeof(char));
		if (code ==  0) throw e_stream_eof(__FILE__,__LINE__,"End of file while reading.");
		if (code == -1) { if (errno == EAGAIN) continue; } // EAGAIN || EINTR ???
		if (code == -1) throw e_stream(__FILE__,__LINE__,std::string()+"Error while read()ing fd.", errno);
		count += code;
		if (count >= size) break;
	}
	// setting timers
	_timing_finish(timer, &deadline);
}

void c_stream::_write (const void * buffer, t_object_size size, timeval * timer)
{
	// prepairing timers
	timeval deadline;
	if (_timing_start(timer, &deadline)) throw e_stream_timeout(__FILE__,__LINE__,"Timeout before write operation (precondition).");
	// writing
	if (!f_fd_defined) throw e_stream(__FILE__,__LINE__,std::string()+"No fd defined in stream.");
	t_object_size count = 0;
	while (true)
	{
		int code;
		// calculating time left for io operations
		timeval timeout;
		if (_timing_cycle(&deadline, &timeout)) throw e_stream_timeout(__FILE__,__LINE__,"Timeout in write operation (in cycle).");
		// waiting for data from file during specified time
		fd_set fdset;
		FD_ZERO(&fdset);
		FD_SET(f_fd, &fdset);
		code = ::select(f_fd+1, NULL, &fdset, NULL, &timeout);
		if (code ==  0) throw e_stream_timeout(__FILE__,__LINE__,"Timeout when selecting for write.");
		if (code == -1) { if (errno == EAGAIN) continue; } // EAGAIN || EINTR ???
		if (code == -1) throw e_stream(__FILE__,__LINE__,std::string()+"Error while select()ing fd for write().", errno);
		// writing data to file if it is ready
		code = ::write(f_fd, static_cast<const char*>(buffer)+count, size-count);
		if (code ==  0) throw e_stream_eof(__FILE__,__LINE__,"End of file while writing.");
		if (code == -1) { if (errno == EAGAIN) continue; } // EAGAIN || EINTR ???
		if (code == -1) throw e_stream(__FILE__,__LINE__,std::string()+"Error while write()ing fd.", errno);
		count += code;
		if (count >= size) break;
	}
	// setting timers
	_timing_finish(timer, &deadline);
}

c_stream::t_fd c_stream::fd_minimize (c_stream::t_fd fd)
{
	return fd; //TODO!!! with use of sysctl(dup) make this file descriptor as small as possible
}
