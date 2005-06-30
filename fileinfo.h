#ifndef _FILESEARCH_FILEINFO_
#define _FILESEARCH_FILEINFO_

#include "config.h"
#include "globals.h"

#include "c_path.h"
#include "c_flag.h"
#include "c_unsigned.h"
#include "c_stamp.h"

class c_fileinfo {
private:
	c_path f_path;
	c_flag f_container;
	c_unsigned f_size;
	c_stamp f_ctime;
	c_stamp f_mtime;
protected:
public:
	c_fileinfo ();
	c_fileinfo (const c_fileinfo & right);
	c_fileinfo (c_path a_path, c_flag a_container, c_unsigned a_size, c_stamp a_ctime, c_stamp a_mtime);

	c_path             path () const;
	c_flag        container () const;
	c_unsigned         size () const;
	c_stamp           ctime () const;
	c_stamp           mtime () const;
};

#endif
