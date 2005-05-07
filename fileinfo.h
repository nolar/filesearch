#ifndef _FILESEARCH_FILEINFO_
#define _FILESEARCH_FILEINFO_

#include <string>
#include <vector>
#include "config.h"
#include "typedefs.h"

class c_fileinfo {
private:
	t_path f_path;
	t_flag f_container;
	t_size f_size;
	t_time f_ctime;
	t_time f_mtime;
protected:
public:
	c_fileinfo ();
	c_fileinfo (t_path      a_path, t_flag      a_container, t_size      a_size, t_time      a_ctime, t_time      a_mtime);
	c_fileinfo (std::string a_path, std::string a_container, std::string a_size, std::string a_ctime, std::string a_mtime);

	c_fileinfo & operator = (const c_fileinfo & right);

	t_path path ();
	std::string name ();
	t_flag container ();
	t_size size ();
	t_time ctime ();
	t_time mtime ();
};

#endif
