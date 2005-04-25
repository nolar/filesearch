#ifndef _FILESEARCH_THREAD_
#define _FILESEARCH_THREAD_

#include <string>
#include <vector>
#include "utils.h"
#include "request.h"
#include "fileinfo.h"

using namespace std;

typedef void (*t_thread_wrap_topaction  ) (c_request);
typedef void (*t_thread_wrap_subaction  ) (c_request, c_fileinfo);
typedef void (*t_thread_wrap_flushaction) (c_request, c_path);
extern void thread_wrap (string command, c_request request, t_thread_wrap_topaction topaction, t_thread_wrap_subaction subaction, t_thread_wrap_flushaction flushaction);
extern void thread_init ();
extern void thread_free ();

#endif
