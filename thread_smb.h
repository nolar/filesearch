#ifndef _FILESEARCH_THREAD_SMB_
#define _FILESEARCH_THREAD_SMB_

#include <string>
#include <vector>
#include "config.h"
#include "typedefs.h"
#include "request.h"

using namespace std;

extern string    thread_smb__command;
extern c_request thread_smb__request;
extern int       thread_smb ();

#endif
