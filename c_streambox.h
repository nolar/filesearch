#ifndef _FILESEARCH_STREAMBOX_
#define _FILESEARCH_STREAMBOX_

#include "config.h"
#include "c_object.h"
#include "e_convert.h"

#include "c_stopsign.h"
#include "c_identifier.h"
#include "c_signed.h"
#include "c_unsigned.h"
#include "c_double.h"
#include "c_flag.h"
#include "c_stamp.h"
#include "c_string.h"
#include "c_ipaddr.h"
#include "c_path.h"
#include "c_event.h"
#include "c_protocol.h"

class c_streambox
{
public:
	// type of value
	typedef unsigned char t_object_type;
	// enumeration possibilities
	static const t_object_type ot_null;
	static const t_object_type ot_stopsign;
	static const t_object_type ot_identifier;
	static const t_object_type ot_signed;
	static const t_object_type ot_unsigned;
	static const t_object_type ot_double;
	static const t_object_type ot_flag;
	static const t_object_type ot_stamp;
	static const t_object_type ot_string;
	static const t_object_type ot_ipaddr;
	static const t_object_type ot_path;
	static const t_object_type ot_event;
	static const t_object_type ot_protocol;
private:
	c_object * f_object;
protected:
	// supplimentary routines
	static c_object *    _create (t_object_type type);
	static t_object_type _typeof (const c_object * object);
	static std::string   _nameof (t_object_type type);
	static std::string   _nameof (const c_object * object);
public:
	// constructors
	c_streambox ();
	c_streambox (const c_streambox & right);
	c_streambox (t_object_type a_type);
	c_streambox (c_object * a_object);
	virtual ~c_streambox ();
	// operators
	c_streambox & operator= (const c_streambox & right);
	// value handling routines
	c_object * get (void) const;
	void set (c_object * value);
	std::string   name () const;
	t_object_type type () const;
	// enumeration routines
	bool is_stopsign   () const; c_stopsign   & as_stopsign   () const;
	bool is_identifier () const; c_identifier & as_identifier () const;
	bool is_signed     () const; c_signed     & as_signed     () const;
	bool is_unsigned   () const; c_unsigned   & as_unsigned   () const;
	bool is_double     () const; c_double     & as_double     () const;
	bool is_flag       () const; c_flag       & as_flag       () const;
	bool is_stamp      () const; c_stamp      & as_stamp      () const;
	bool is_string     () const; c_string     & as_string     () const;
	bool is_ipaddr     () const; c_ipaddr     & as_ipaddr     () const;
	bool is_path       () const; c_path       & as_path       () const;
	bool is_event      () const; c_event      & as_event      () const;
	bool is_protocol   () const; c_protocol   & as_protocol   () const;
};

#endif
