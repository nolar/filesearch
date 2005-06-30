#ifndef _FILESEARCH_OBJECTS_PROTOCOL_
#define _FILESEARCH_OBJECTS_PROTOCOL_

#include "config.h"
#include "c_object.h"
#include "e_convert.h"
#include <string>

class c_protocol : public c_object
{
public:
	typedef unsigned char t_value;
	static const t_value value_smb;
	static const t_value value_ftp;
	static const t_value value_http;
private:
	t_value f_value;
protected:
public:
	// constructors
	c_protocol ();
	c_protocol (const c_protocol & right);
	c_protocol (t_value value);
	// streaming routines
	virtual bool stream_vary () const;
	virtual t_object_size stream_size () const;
	virtual void stream_getdata (void * buffer, t_object_size size) const;
	virtual void stream_setdata (const void * buffer, t_object_size size);
	// operators
	c_protocol & operator= (const c_protocol & right);
	bool operator== (const c_protocol & right) const;
	bool operator!= (const c_protocol & right) const;
	// value handling routines
	t_value get (void) const;
	void set (t_value value);
	std::string ascii () const;
	// enumeration routines
	bool is_smb  () const;
	bool is_ftp  () const;
	bool is_http () const;
};

#endif
