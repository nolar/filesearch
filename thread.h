#ifndef _FILESEARCH_THREAD_
#define _FILESEARCH_THREAD_

#include <string>
#include <vector>
#include "utils.h"
#include "request.h"
#include "fileinfo.h"

using namespace std;

typedef void (*t_thread_wrap_action_resource) (c_request, c_path);
typedef void (*t_thread_wrap_action_dir     ) (c_request, c_fileinfo);
typedef void (*t_thread_wrap_action_file    ) (c_request, c_fileinfo);
typedef void (*t_thread_wrap_action_enter   ) (c_request, c_path);
typedef void (*t_thread_wrap_action_leave   ) (c_request, c_path);
typedef void (*t_thread_wrap_action_start   ) (c_request);
extern void thread_init ();
extern void thread_free ();
extern bool thread_wrap (string command, c_request request,
	t_thread_wrap_action_resource action_resource,
	t_thread_wrap_action_dir      action_dir,
	t_thread_wrap_action_file     action_file,
	t_thread_wrap_action_enter    action_enter,
	t_thread_wrap_action_leave    action_leave,
	t_thread_wrap_action_start    action_start);

#endif
