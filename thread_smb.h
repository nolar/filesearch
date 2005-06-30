#ifndef _FILESEARCH_THREAD_SMB_
#define _FILESEARCH_THREAD_SMB_

#include <string>
#include <vector>
#include "config.h"
#include "globals.h"
#include "query.h"

extern std::string thread_smb__command;
extern c_query   thread_smb__query;
extern int         thread_smb ();

#endif
