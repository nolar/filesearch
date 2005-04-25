#ifndef _FILESEARCH_THREAD_SMB_
#define _FILESEARCH_THREAD_SMB_

#include <string>
#include <vector>
#include "utils.h"
#include "request.h"
#include "fileinfo.h"

using namespace std;

extern string    thread_smb__command;
extern c_request thread_smb__request;
extern int       thread_smb ();

#endif
