#ifndef _FILESEARCH_CONVERT_
#define _FILESEARCH_CONVERT_

#include <string>
#include <vector>
#include "config.h"
#include "typedefs.h"
#include "e_convert.h"

class convert
{
private:
protected:
	static const int _buffer_size;
	static const char * _time_format;

	static std::string _sprintf (const char * format, ...);
	static std::string _strftime (const char * format, time_t value);
	static std::vector<std::string> _split (std::string value, std::string delimiter);
public:
	// convertions for basic (atomic) types
	static unsigned       int str2ui  (std::string value);
	static   signed       int str2si  (std::string value);
	static unsigned      long str2ul  (std::string value);
	static   signed      long str2sl  (std::string value);
	static unsigned long long str2ull (std::string value);
	static   signed long long str2sll (std::string value);
	static              float str2f   (std::string value);
	static             double str2d   (std::string value);
	static        long double str2ld  (std::string value);

	static std::string ui2str  (unsigned       int value, int pad = 0);
	static std::string si2str  (  signed       int value, int pad = 0);
	static std::string ul2str  (unsigned      long value, int pad = 0);
	static std::string sl2str  (  signed      long value, int pad = 0);
	static std::string ull2str (unsigned long long value, int pad = 0);
	static std::string sll2str (  signed long long value, int pad = 0);
	static std::string   f2str (             float value, int pad = 0, int precision = 6);
	static std::string   d2str (            double value, int pad = 0, int precision = 6);
	static std::string  ld2str (       long double value, int pad = 0, int precision = 6);

	///////////////////////////////////////////////////////////////////////////////////////////
	// conversions for application types
	//
	// ipc2<type>(str)	- for data received from scanner processes (throws on error)
	// <type>2ipc		- for data transferred to scanner processes in tasks
	//
	// <type>2print	- for user-friendly output (debug & log messages).
	///////////////////////////////////////////////////////////////////////////////////////////
	static t_ipaddr  ipcval2ipaddr  (t_ipc_val value);
	static t_ipmask  ipcval2ipmask  (t_ipc_val value);
	static t_ipport  ipcval2ipport  (t_ipc_val value);
	static t_proto   ipcval2proto   (t_ipc_val value);
	static t_path    ipcval2path    (t_ipc_val value);
	static t_time    ipcval2time    (t_ipc_val value);
	static t_size    ipcval2size    (t_ipc_val value);
//	static t_sqlid   ipcval2sqlid   (t_ipc_val value);//???
//	static t_pident  ipcval2pident  (t_ipc_val value);//???
//	static t_pstatus ipcval2pstatus (t_ipc_val value);//???
//	static t_fd      ipcval2fd      (t_ipc_val value);//???
	static t_flag    ipcval2flag    (t_ipc_val value);
	static t_timeout ipcval2timeout (t_ipc_val value);
	static t_depth   ipcval2depth   (t_ipc_val value);
/*
	static t_ipaddr  auto2ipaddr  (std::string value);
	static t_ipmask  auto2ipmask  (std::string value);
	static t_ipport  auto2ipport  (std::string value);
	static t_proto   auto2proto   (std::string value);
	static t_path    auto2path    (std::string value);
	static t_time    auto2time    (std::string value);
	static t_size    auto2size    (std::string value);
	static t_sqlid   auto2sqlid   (std::string value);
	static t_pident  auto2pident  (std::string value);
	static t_pstatus auto2pstatus (std::string value);
	static t_fd      auto2fd      (std::string value);
	static t_flag    auto2flag    (std::string value);
	static t_timeout auto2timeout (std::string value);
	static t_depth   auto2depth   (std::string value);

	static t_ipaddr  auto2ipaddr  (std::string value, t_ipaddr  def);
	static t_ipmask  auto2ipmask  (std::string value, t_ipmask  def);
	static t_ipport  auto2ipport  (std::string value, t_ipport  def);
	static t_proto   auto2proto   (std::string value, t_proto   def);
	static t_path    auto2path    (std::string value, t_path    def);
	static t_time    auto2time    (std::string value, t_time    def);
	static t_size    auto2size    (std::string value, t_size    def);
	static t_sqlid   auto2sqlid   (std::string value, t_sqlid   def);
	static t_pident  auto2pident  (std::string value, t_pident  def);
	static t_pstatus auto2pstatus (std::string value, t_pstatus def);
	static t_fd      auto2fd      (std::string value, t_fd      def);
	static t_flag    auto2flag    (std::string value, t_flag    def);
	static t_timeout auto2timeout (std::string value, t_timeout def);
	static t_depth   auto2depth   (std::string value, t_depth   def);
*/
	static std::string  ipaddr2print  (t_ipaddr  value);
	static std::string  ipmask2print  (t_ipmask  value);
	static std::string  ipport2print  (t_ipport  value);
	static std::string   proto2print  (t_proto   value);
	static std::string    path2print  (t_path    value);
	static std::string    time2print  (t_time    value, std::string format = "");
	static std::string    size2print  (t_size    value, int pad = 0);
//	static std::string   sqlid2print  (t_sqlid   value, int pad = 0);
//	static std::string  pident2print  (t_pident  value, int pad = 0);
//	static std::string pstatus2print  (t_pstatus value);
//	static std::string      fd2print  (t_fd      value, int pad = 0);
	static std::string    flag2print  (t_flag    value, std::string t = "true", std::string f = "false");
	static std::string timeout2print  (t_timeout value, int pad = 0);
	static std::string   depth2print  (t_depth   value, int pad = 0);

/*	static t_ipc_val  ipaddr2ipcval (t_ipaddr  value);
	static t_ipc_val  ipmask2ipcval (t_ipmask  value);
	static t_ipc_val  ipport2ipcval (t_ipport  value);
	static t_ipc_val   proto2ipcval (t_proto   value);
	static t_ipc_val    path2ipcval (t_path    value);
	static t_ipc_val    time2ipcval (t_time    value);
	static t_ipc_val    size2ipcval (t_size    value);
	static t_ipc_val   sqlid2ipcval (t_sqlid   value);
	static t_ipc_val  pident2ipcval (t_pident  value);
	static t_ipc_val pstatus2ipcval (t_pstatus value);
	static t_ipc_val      fd2ipcval (t_fd      value);
	static t_ipc_val    flag2ipcval (t_flag    value);
	static t_ipc_val timeout2ipcval (t_timeout value);
	static t_ipc_val   depth2ipcval (t_depth   value);*/
};

#endif
