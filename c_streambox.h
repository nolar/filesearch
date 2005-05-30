#ifndef _FILESEARCH_STREAMBOX_
#define _FILESEARCH_STREAMBOX_

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
#include "c_action.h"
#include "c_protocol.h"

class c_streambox
{
public:
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
	c_object * f_object;
protected:
	static c_object *    _create (t_object_type type);
	static t_object_type _typeof (const c_object * object);
	static const char *  _nameof (t_object_type type);
	static const char *  _nameof (const c_object * object);
public:
	c_streambox ();
	c_streambox (c_streambox & right);
	c_streambox (t_object_type a_type);
	c_streambox (c_object * a_object);
	virtual ~c_streambox ();

	t_object_type type   () const;
	const char *  name   () const;
	c_object *    object () const;

	c_stopsign & as_stopsign () const;
	c_mapkey   & as_mapkey   () const;
	c_signed   & as_signed   () const;
	c_unsigned & as_unsigned () const;
	c_double   & as_double   () const;
	c_flag     & as_flag     () const;
	c_stamp    & as_stamp    () const;
	c_string   & as_string   () const;
	c_ipaddr   & as_ipaddr   () const;
	c_path     & as_path     () const;
	c_action   & as_action   () const;
	c_protocol & as_protocol () const;
};

#endif
