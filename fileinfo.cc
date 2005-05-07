#include "fileinfo.h"
#include "convert.h"

c_fileinfo::c_fileinfo ()
{
}

c_fileinfo::c_fileinfo (t_path a_path, t_flag a_container, t_size a_size, t_time a_ctime, t_time a_mtime)
{
	f_path      = a_path;
	f_container = a_container;
	f_size      = a_size;
	f_ctime     = a_ctime;
	f_mtime     = a_mtime;
}

c_fileinfo::c_fileinfo (std::string a_path, std::string a_container, std::string a_size, std::string a_ctime, std::string a_mtime)
{
	f_path      = convert::str2path(a_path);
	f_container = convert::str2flag(a_container);
	f_size      = convert::str2size(a_size);
	f_ctime     = convert::str2time(a_ctime);
	f_mtime     = convert::str2time(a_mtime);
}

c_fileinfo & c_fileinfo::operator= (const c_fileinfo & right)
{
	f_path      = right.f_path;
	f_container = right.f_container;
	f_size      = right.f_size;
	f_ctime     = right.f_ctime;
	f_mtime     = right.f_mtime;
	return *this;
}



t_path c_fileinfo::path ()
{
	return f_path;
}

std::string c_fileinfo::name ()
{
	return f_path.empty() ? std::string() : f_path[f_path.size()-1];
}

t_flag c_fileinfo::container ()
{
	return f_container;
}

t_size c_fileinfo::size ()
{
	return f_size;
}

t_time c_fileinfo::ctime ()
{
	return f_ctime;
}

t_time c_fileinfo::mtime ()
{
	return f_mtime;
}
