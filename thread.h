#ifndef _FILESEARCH_THREAD_
#define _FILESEARCH_THREAD_

#include <string>
#include <exception>

#include "config.h"
#include "globals.h"
#include "request.h"
#include "fileinfo.h"
#include "e_basic.h"

typedef void (*t_wrap_action_resource) (c_request, t_path);
typedef void (*t_wrap_action_dir     ) (c_request, c_fileinfo);
typedef void (*t_wrap_action_file    ) (c_request, c_fileinfo);
typedef void (*t_wrap_action_enter   ) (c_request, t_path);
typedef void (*t_wrap_action_leave   ) (c_request, t_path);
typedef void (*t_wrap_action_start   ) (c_request);
extern void thread_init ();
extern void thread_free ();
extern int  thread_catch (std::exception*, e_basic*);
extern bool thread_wrap (std::string command, c_request request,
	t_wrap_action_resource action_resource,
	t_wrap_action_dir      action_dir,
	t_wrap_action_file     action_file,
	t_wrap_action_enter    action_enter,
	t_wrap_action_leave    action_leave,
	t_wrap_action_start    action_start);

#endif
