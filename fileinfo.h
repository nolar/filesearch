#ifndef _FILESEARCH_FILEINFO_
#define _FILESEARCH_FILEINFO_

#include <string>
#include <vector>
#include "utils.h"

using namespace std;

class c_fileinfo {
private:
	c_path f_path;
	bool f_container;
	size_t f_size;
	time_t f_ctime;
	time_t f_mtime;
protected:
public:
	c_fileinfo ();
	c_fileinfo (c_path a_path, bool a_container, size_t a_size, time_t a_ctime, time_t a_mtime);
	c_fileinfo (string a_path, string a_container, string a_size, string a_ctime, string a_mtime);

	c_fileinfo & operator = (const c_fileinfo & right);

	c_path path ();
	string name ();
	bool container ();
	size_t size ();
	time_t ctime ();
	time_t mtime ();
};

#endif
