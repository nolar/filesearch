#include "fileinfo.h"

c_fileinfo::c_fileinfo ()
{
}

c_fileinfo::c_fileinfo (const c_fileinfo & right)
{
	f_path      = right.f_path;
	f_container = right.f_container;
	f_size      = right.f_size;
	f_ctime     = right.f_ctime;
	f_mtime     = right.f_mtime;
}

c_fileinfo::c_fileinfo (c_path a_path, c_flag a_container, c_unsigned a_size, c_stamp a_ctime, c_stamp a_mtime)
{
	f_path      = a_path;
	f_container = a_container;
	f_size      = a_size;
	f_ctime     = a_ctime;
	f_mtime     = a_mtime;
}





c_path c_fileinfo::path () const
{
	return f_path;
}

c_flag c_fileinfo::container () const
{
	return f_container;
}

c_unsigned c_fileinfo::size () const
{
	return f_size;
}

c_stamp c_fileinfo::ctime () const
{
	return f_ctime;
}

c_stamp c_fileinfo::mtime () const
{
	return f_mtime;
}
