#include "options.h"

unsigned options::default_timeout = 5;
unsigned options::default_depth = 3; //!!! change to infinite

string options::command_smb = "./filesearch-smb";

string::value_type options::terminator = '\n';
long options::timeout_params_sec  = 1;
long options::timeout_params_usec = 0;


string options::action_code_for_resource = "@resource";
string options::action_code_for_dir      = "*dir";
string options::action_code_for_file     = "+file";
string options::action_code_for_enter    = ">enter";
string options::action_code_for_leave    = "<leave";

int options::fd_task  = 0;
int options::fd_data  = 3;
int options::fd_log   = 2;
int options::fd_debug = 1;
int options::fd_null  = -1;

char * options::log_time_format = "%Y-%m-%d %H:%M:%S";
int    options::log_pid_length = 5;
