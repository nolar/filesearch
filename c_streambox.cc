#include "c_streambox.h"
#include "globals.h"
//#include "c_stream.h"





const c_streambox::t_object_type c_streambox::ot_null       = ~0;
const c_streambox::t_object_type c_streambox::ot_stopsign   =  0;
const c_streambox::t_object_type c_streambox::ot_identifier =  1;
const c_streambox::t_object_type c_streambox::ot_signed     =  2;
const c_streambox::t_object_type c_streambox::ot_unsigned   =  3;
const c_streambox::t_object_type c_streambox::ot_double     =  4;
const c_streambox::t_object_type c_streambox::ot_flag       =  5;
const c_streambox::t_object_type c_streambox::ot_stamp      =  6;
const c_streambox::t_object_type c_streambox::ot_string     =  7;
const c_streambox::t_object_type c_streambox::ot_ipaddr     =  8;
const c_streambox::t_object_type c_streambox::ot_path       =  9;
const c_streambox::t_object_type c_streambox::ot_event      = 10;
const c_streambox::t_object_type c_streambox::ot_protocol   = 11;





c_streambox::c_streambox ()
	: f_object(NULL)
{
}

c_streambox::c_streambox (const c_streambox & right)
	: f_object(NULL)
{
	set(right.f_object);
}

c_streambox::c_streambox (c_streambox::t_object_type type)
	: f_object(NULL)
{
	set(_create(type));
}

c_streambox::c_streambox (c_object * a_object)
	: f_object(NULL)
{
	set(a_object);
}

c_streambox::~c_streambox ()
{
	set(NULL);
}





c_streambox & c_streambox::operator= (const c_streambox & right)
{
	set(right.f_object);
	return *this;
}





c_object * c_streambox::get () const
{
	return f_object;
}

void c_streambox::set (c_object * value)
{
	if (f_object)
	{
		f_object->reference_decrease();
		if (f_object->reference_zero())
			delete f_object;
	}
	f_object = value;
	if (f_object)
	{
		f_object->reference_increase();
	}
}

std::string c_streambox::name () const
{
	return _nameof(f_object);
}

c_streambox::t_object_type c_streambox::type () const
{
	return _typeof(f_object);
}





bool c_streambox::is_stopsign () const
{
	return dynamic_cast<c_stopsign*>(f_object) != NULL;
}

bool c_streambox::is_identifier () const
{
	return dynamic_cast<c_identifier*>(f_object) != NULL;
}

bool c_streambox::is_signed () const
{
	return dynamic_cast<c_signed*>(f_object) != NULL;
}

bool c_streambox::is_unsigned () const
{
	return dynamic_cast<c_unsigned*>(f_object) != NULL;
}

bool c_streambox::is_double () const
{
	return dynamic_cast<c_double*>(f_object) != NULL;
}

bool c_streambox::is_flag () const
{
	return dynamic_cast<c_flag*>(f_object) != NULL;
}

bool c_streambox::is_stamp () const
{
	return dynamic_cast<c_stamp*>(f_object) != NULL;
}

bool c_streambox::is_string () const
{
	return dynamic_cast<c_string*>(f_object) != NULL;
}

bool c_streambox::is_ipaddr () const
{
	return dynamic_cast<c_ipaddr*>(f_object) != NULL;
}

bool c_streambox::is_path () const
{
	return dynamic_cast<c_path*>(f_object) != NULL;
}

bool c_streambox::is_event () const
{
	return dynamic_cast<c_event*>(f_object) != NULL;
}

bool c_streambox::is_protocol () const
{
	return dynamic_cast<c_protocol*>(f_object) != NULL;
}





c_stopsign & c_streambox::as_stopsign () const
{
	c_stopsign * temp = dynamic_cast<c_stopsign*>(f_object);
	if (!temp) throw e_convert(__FILE__,__LINE__,std::string()+"Trying to cast object '"+name()+"' to 'stopsign'.");
	return *temp;
}

c_identifier & c_streambox::as_identifier () const
{
	c_identifier * temp = dynamic_cast<c_identifier*>(f_object);
	if (!temp) throw e_convert(__FILE__,__LINE__,std::string()+"Trying to cast object '"+name()+"' to 'identifier'.");
	return *temp;
}

c_signed & c_streambox::as_signed () const
{
	c_signed * temp = dynamic_cast<c_signed*>(f_object);
	if (!temp) throw e_convert(__FILE__,__LINE__,std::string()+"Trying to cast object '"+name()+"' to 'signed'.");
	return *temp;
}

c_unsigned & c_streambox::as_unsigned () const
{
	c_unsigned * temp = dynamic_cast<c_unsigned*>(f_object);
	if (!temp) throw e_convert(__FILE__,__LINE__,std::string()+"Trying to cast object '"+name()+"' to 'unsigned'.");
	return *temp;
}

c_double & c_streambox::as_double () const
{
	c_double * temp = dynamic_cast<c_double*>(f_object);
	if (!temp) throw e_convert(__FILE__,__LINE__,std::string()+"Trying to cast object '"+name()+"' to 'double'.");
	return *temp;
}

c_flag & c_streambox::as_flag () const
{
	c_flag * temp = dynamic_cast<c_flag*>(f_object);
	if (!temp) throw e_convert(__FILE__,__LINE__,std::string()+"Trying to cast object '"+name()+"' to 'flag'.");
	return *temp;
}

c_stamp & c_streambox::as_stamp () const
{
	c_stamp * temp = dynamic_cast<c_stamp*>(f_object);
	if (!temp) throw e_convert(__FILE__,__LINE__,std::string()+"Trying to cast object '"+name()+"' to 'stamp'.");
	return *temp;
}

c_string & c_streambox::as_string () const
{
	c_string * temp = dynamic_cast<c_string*>(f_object);
	if (!temp) throw e_convert(__FILE__,__LINE__,std::string()+"Trying to cast object '"+name()+"' to 'string'.");
	return *temp;
}

c_ipaddr & c_streambox::as_ipaddr () const
{
	c_ipaddr * temp = dynamic_cast<c_ipaddr*>(f_object);
	if (!temp) throw e_convert(__FILE__,__LINE__,std::string()+"Trying to cast object '"+name()+"' to 'ipaddr'.");
	return *temp;
}

c_path & c_streambox::as_path () const
{
	c_path * temp = dynamic_cast<c_path*>(f_object);
	if (!temp) throw e_convert(__FILE__,__LINE__,std::string()+"Trying to cast object '"+name()+"' to 'path'.");
	return *temp;
}

c_event & c_streambox::as_event () const
{
	c_event * temp = dynamic_cast<c_event*>(f_object);
	if (!temp) throw e_convert(__FILE__,__LINE__,std::string()+"Trying to cast object '"+name()+"' to 'event'.");
	return *temp;
}

c_protocol & c_streambox::as_protocol () const
{
	c_protocol * temp = dynamic_cast<c_protocol*>(f_object);
	if (!temp) throw e_convert(__FILE__,__LINE__,std::string()+"Trying to cast object '"+name()+"' to 'protocol'.");
	return *temp;
}





c_object * c_streambox::_create (c_streambox::t_object_type type)
{
	switch (type)
	{
		case ot_stopsign  : return new c_stopsign  ();
		case ot_identifier: return new c_identifier();
		case ot_signed    : return new c_signed    ();
		case ot_unsigned  : return new c_unsigned  ();
		case ot_double    : return new c_double    ();
		case ot_flag      : return new c_flag      ();
		case ot_stamp     : return new c_stamp     ();
		case ot_string    : return new c_string    ();
		case ot_ipaddr    : return new c_ipaddr    ();
		case ot_path      : return new c_path      ();
		case ot_event     : return new c_event     ();
		case ot_protocol  : return new c_protocol  ();
		default: return NULL;
	}
}

c_streambox::t_object_type c_streambox::_typeof (const c_object * object)
{
	if (object == NULL) return ot_null; else
	if (dynamic_cast<const c_stopsign  *>(object)) return ot_stopsign  ; else
	if (dynamic_cast<const c_identifier*>(object)) return ot_identifier; else
	if (dynamic_cast<const c_signed    *>(object)) return ot_signed    ; else
	if (dynamic_cast<const c_unsigned  *>(object)) return ot_unsigned  ; else
	if (dynamic_cast<const c_double    *>(object)) return ot_double    ; else
	if (dynamic_cast<const c_flag      *>(object)) return ot_flag      ; else
	if (dynamic_cast<const c_stamp     *>(object)) return ot_stamp     ; else
	if (dynamic_cast<const c_string    *>(object)) return ot_string    ; else
	if (dynamic_cast<const c_ipaddr    *>(object)) return ot_ipaddr    ; else
	if (dynamic_cast<const c_path      *>(object)) return ot_path      ; else
	if (dynamic_cast<const c_event     *>(object)) return ot_event     ; else
	if (dynamic_cast<const c_protocol  *>(object)) return ot_protocol  ; else
	return ot_null;
}

std::string c_streambox::_nameof (c_streambox::t_object_type type)
{
	switch (type)
	{
		case ot_stopsign  : return std::string("stopsign"  );
		case ot_identifier: return std::string("identifier");
		case ot_signed    : return std::string("signed"    );
		case ot_unsigned  : return std::string("unsigned"  );
		case ot_double    : return std::string("double"    );
		case ot_flag      : return std::string("flag"      );
		case ot_stamp     : return std::string("stamp"     );
		case ot_string    : return std::string("string"    );
		case ot_ipaddr    : return std::string("ipaddr"    );
		case ot_path      : return std::string("path"      );
		case ot_event     : return std::string("event"     );
		case ot_protocol  : return std::string("protocol"  );
		default: return std::string("unknown") + "(" + string_format("%d", type) + ")";
	}
}

std::string c_streambox::_nameof (const c_object * object)
{
	return _nameof(_typeof(object));
}
