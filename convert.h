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

	static t_ipaddr  str2ipaddr  (std::string value);
	static t_ipmask  str2ipmask  (std::string value);
	static t_ipport  str2ipport  (std::string value);
	static t_proto   str2proto   (std::string value);
	static t_path    str2path    (std::string value);
	static t_time    str2time    (std::string value);
	static t_size    str2size    (std::string value);
	static t_sqlid   str2sqlid   (std::string value);
	static t_pident  str2pident  (std::string value);
	static t_pstatus str2pstatus (std::string value);
	static t_fd      str2fd      (std::string value);
	static t_flag    str2flag    (std::string value);
	static t_timeout str2timeout (std::string value);
	static t_depth   str2depth   (std::string value);

	static t_ipaddr  str2ipaddr  (std::string value, t_ipaddr  def);
	static t_ipmask  str2ipmask  (std::string value, t_ipmask  def);
	static t_ipport  str2ipport  (std::string value, t_ipport  def);
	static t_proto   str2proto   (std::string value, t_proto   def);
	static t_path    str2path    (std::string value, t_path    def);
	static t_time    str2time    (std::string value, t_time    def);
	static t_size    str2size    (std::string value, t_size    def);
	static t_sqlid   str2sqlid   (std::string value, t_sqlid   def);
	static t_pident  str2pident  (std::string value, t_pident  def);
	static t_pstatus str2pstatus (std::string value, t_pstatus def);
	static t_fd      str2fd      (std::string value, t_fd      def);
	static t_flag    str2flag    (std::string value, t_flag    def);
	static t_timeout str2timeout (std::string value, t_timeout def);
	static t_depth   str2depth   (std::string value, t_depth   def);

	static std::string  ipaddr2print  (t_ipaddr  value);
	static std::string  ipmask2print  (t_ipmask  value);
	static std::string  ipport2print  (t_ipport  value);
	static std::string   proto2print  (t_proto   value);
	static std::string    path2print  (t_path    value);
	static std::string    time2print  (t_time    value, std::string format = "");
	static std::string    size2print  (t_size    value, int pad = 0);
	static std::string   sqlid2print  (t_sqlid   value, int pad = 0);
	static std::string  pident2print  (t_pident  value, int pad = 0);
	static std::string pstatus2print  (t_pstatus value);
	static std::string      fd2print  (t_fd      value, int pad = 0);
	static std::string    flag2print  (t_flag    value, std::string t = "true", std::string f = "false");
	static std::string timeout2print  (t_timeout value, int pad = 0);
	static std::string   depth2print  (t_depth   value, int pad = 0);

	static std::string  ipaddr2system (t_ipaddr  value);
	static std::string  ipmask2system (t_ipmask  value);
	static std::string  ipport2system (t_ipport  value);
	static std::string   proto2system (t_proto   value);
	static std::string    path2system (t_path    value);
	static std::string    time2system (t_time    value);
	static std::string    size2system (t_size    value);
	static std::string   sqlid2system (t_sqlid   value);
	static std::string  pident2system (t_pident  value);
	static std::string pstatus2system (t_pstatus value);
	static std::string      fd2system (t_fd      value);
	static std::string    flag2system (t_flag    value);
	static std::string timeout2system (t_timeout value);
	static std::string   depth2system (t_depth   value);
};

#endif
