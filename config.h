#ifndef _FILESEARCH_CONFIG_
#define _FILESEARCH_CONFIG_

// uncomment this to disable debugging messages
//#define NODEBUG

//
//#define NOLOG

//
//#define NOSTATUS



////////////////////////////////////////////////////////////////////////////////////////////////////
//                                    DO NOT EDIT WHAT FOLLOWS                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef NOLOG
#define LOG(s)
#define IFLOG(s)
#else
#define LOG(s) { s_log.stamped(std::string()+s); DEBUG(s); }
#define IFLOG(s) s
#endif

#ifdef NODEBUG
#define DEBUG(s)
#define IFDEBUG(s)
#else
#define DEBUG(s) { s_debug.stamped(std::string()+s); }
#define IFDEBUG(s) s
#endif

#ifdef NOSTATUS
#define STATUS(s)
#define IFSTATUS(s)
#else
#define STATUS(s) { setproctitle("%s", (std::string()+s).c_str()); }
#define IFSTATUS(s) s
#endif

#endif
