#include "c_streambox.h"



const c_streambox::t_object_type c_streambox::ot_null     = ~0;
const c_streambox::t_object_type c_streambox::ot_stopsign =  0;
const c_streambox::t_object_type c_streambox::ot_mapkey   =  1;
const c_streambox::t_object_type c_streambox::ot_signed   =  2;
const c_streambox::t_object_type c_streambox::ot_unsigned =  3;
const c_streambox::t_object_type c_streambox::ot_double   =  4;
const c_streambox::t_object_type c_streambox::ot_flag     =  5;
const c_streambox::t_object_type c_streambox::ot_stamp    =  6;
const c_streambox::t_object_type c_streambox::ot_string   =  7;
const c_streambox::t_object_type c_streambox::ot_ipaddr   =  8;
const c_streambox::t_object_type c_streambox::ot_path     =  9;
const c_streambox::t_object_type c_streambox::ot_action   = 10;
const c_streambox::t_object_type c_streambox::ot_protocol = 11;



c_streambox::c_streambox ()
{
	f_object = NULL;
}

c_streambox::c_streambox (c_streambox & right)
{
	f_object = right.f_object;
	if (f_object) f_object->reference_increase();
}

c_streambox::c_streambox (c_streambox::t_object_type type)
{
	f_object = _create(type);
	if (f_object) f_object->reference_increase();
}

c_streambox::c_streambox (c_object * a_object)
{
	f_object = a_object;
	if (f_object) f_object->reference_increase();
}

c_streambox::~c_streambox ()
{
	if (f_object)
	{
		f_object->reference_decrease();
		if (f_object->reference_zero())
		{
			delete f_object;
			f_object = NULL;
		}
	}
}



c_object * c_streambox::_create (c_streambox::t_object_type type)
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
		case ot_action  : return new c_action  ;
		case ot_protocol: return new c_protocol;
		default: return NULL;
	}
}

c_streambox::t_object_type c_streambox::_typeof (const c_object * object)
{
	if (object == NULL) return ot_null; else
	if (dynamic_cast<const c_stopsign*>(object)) return ot_stopsign; else
	if (dynamic_cast<const c_mapkey  *>(object)) return ot_mapkey  ; else
	if (dynamic_cast<const c_signed  *>(object)) return ot_signed  ; else
	if (dynamic_cast<const c_unsigned*>(object)) return ot_unsigned; else
	if (dynamic_cast<const c_double  *>(object)) return ot_double  ; else
	if (dynamic_cast<const c_flag    *>(object)) return ot_flag    ; else
	if (dynamic_cast<const c_stamp   *>(object)) return ot_stamp   ; else
	if (dynamic_cast<const c_string  *>(object)) return ot_string  ; else
	if (dynamic_cast<const c_ipaddr  *>(object)) return ot_ipaddr  ; else
	if (dynamic_cast<const c_path    *>(object)) return ot_path    ; else
	if (dynamic_cast<const c_action  *>(object)) return ot_action  ; else
	if (dynamic_cast<const c_protocol*>(object)) return ot_protocol; else
	return ot_null;
}

const char * c_streambox::_nameof (c_streambox::t_object_type type)
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
		case ot_action  : return "action"  ;
		case ot_protocol: return "protocol";
		default: return "null(unknown)";
	}
}

const char * c_streambox::_nameof (const c_object * object)
{
	return _nameof(_typeof(object));
}





c_streambox::t_object_type c_streambox::type () const
{
	return _typeof(f_object);
}

const char * c_streambox::name () const
{
	return _nameof(f_object);
}

c_object * c_streambox::object () const
{
	return f_object;
}





c_stopsign & c_streambox::as_stopsign () const
{
	c_stopsign * temp = dynamic_cast<c_stopsign*>(f_object);
	if (!temp) throw 1; //e_cast_error!!!
	return *temp;
}

c_mapkey & c_streambox::as_mapkey () const
{
	c_mapkey * temp = dynamic_cast<c_mapkey*>(f_object);
	if (!temp) throw 1; //e_cast_error!!!
	return *temp;
}

c_signed & c_streambox::as_signed () const
{
	c_signed * temp = dynamic_cast<c_signed*>(f_object);
	if (!temp) throw 1; //e_cast_error!!!
	return *temp;
}

c_unsigned & c_streambox::as_unsigned () const
{
	c_unsigned * temp = dynamic_cast<c_unsigned*>(f_object);
	if (!temp) throw 1; //e_cast_error!!!
	return *temp;
}

c_double & c_streambox::as_double () const
{
	c_double * temp = dynamic_cast<c_double*>(f_object);
	if (!temp) throw 1; //e_cast_error!!!
	return *temp;
}

c_flag & c_streambox::as_flag () const
{
	c_flag * temp = dynamic_cast<c_flag*>(f_object);
	if (!temp) throw 1; //e_cast_error!!!
	return *temp;
}

c_stamp & c_streambox::as_stamp () const
{
	c_stamp * temp = dynamic_cast<c_stamp*>(f_object);
	if (!temp) throw 1; //e_cast_error!!!
	return *temp;
}

c_string & c_streambox::as_string () const
{
	c_string * temp = dynamic_cast<c_string*>(f_object);
	if (!temp) throw 1; //e_cast_error!!!
	return *temp;
}

c_ipaddr & c_streambox::as_ipaddr () const
{
	c_ipaddr * temp = dynamic_cast<c_ipaddr*>(f_object);
	if (!temp) throw 1; //e_cast_error!!!
	return *temp;
}

c_path & c_streambox::as_path () const
{
	c_path * temp = dynamic_cast<c_path*>(f_object);
	if (!temp) throw 1; //e_cast_error!!!
	return *temp;
}

c_action & c_streambox::as_action () const
{
	c_action * temp = dynamic_cast<c_action*>(f_object);
	if (!temp) throw 1; //e_cast_error!!!
	return *temp;
}

c_protocol & c_streambox::as_protocol () const
{
	c_protocol * temp = dynamic_cast<c_protocol*>(f_object);
	if (!temp) throw 1; //e_cast_error!!!
	return *temp;
}
