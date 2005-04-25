#include "fileinfo.h"
//!!!#include <iostream>
//!!!#include <iomanip>
#include "options.h"

c_fileinfo::c_fileinfo ()
{
}

c_fileinfo::c_fileinfo (c_path a_path, bool a_container, size_t a_size, time_t a_ctime, time_t a_mtime)
{
	f_path      = a_path;
	f_container = a_container;
	f_size      = a_size;
	f_ctime     = a_ctime;
	f_mtime     = a_mtime;
}

c_fileinfo::c_fileinfo (string a_path, string a_container, string a_size, string a_ctime, string a_mtime)
{
	f_path      = utils::string2path(a_path);
	f_container = (a_container=="1");
	f_size      = utils::strtoul(a_size);
	f_ctime     = utils::strtoul(a_ctime);
	f_mtime     = utils::strtoul(a_mtime);
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



c_path c_fileinfo::path ()
{
	return f_path;
}

string c_fileinfo::name ()
{
	return f_path.empty() ? "" : f_path[f_path.size()-1];
}

bool c_fileinfo::container ()
{
	return f_container;
}

size_t c_fileinfo::size ()
{
	return f_size;
}

time_t c_fileinfo::ctime ()
{
	return f_ctime;
}

time_t c_fileinfo::mtime ()
{
	return f_mtime;
}
