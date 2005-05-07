#ifndef _FILESEARCH_CONFIG_
#define _FILESEARCH_CONFIG_

// uncomment this to disable debugging messages
//#define NODEBUG



////////////////////////////////////////////////////////////////////////////////////////////////////
//                                    DO NOT EDIT WHAT FOLLOWS                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef NODEBUG
#define DEBUG(s)
#else
#define DEBUG(s) do { io::debug(std::string()+s);         } while (0)
#endif

#define LOG(s)   do { io::log(std::string()+s); DEBUG(s); } while (0)

#define NOP if(0){}

#endif
