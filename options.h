#ifndef _FILESEARCH_OPTIONS_
#define _FILESEARCH_OPTIONS_

#include <string>
#include <vector>

using namespace std;

class options
{
private:
protected:
public:
	static unsigned default_timeout;
	static unsigned default_depth;

	static string command_smb;
};

#endif
