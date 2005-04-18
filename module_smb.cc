#include "e_database.h"
#include "database.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "fdstream"
#include "utils.h"

extern c_database * database;

// configs to scanner (global)
string module_smb__cmd_for_host("./scan-host");
string module_smb__cmd_for_share("./scan-share");




//
int smb_scan_host (c_request request)
{
	// creating fd for stdin & stdout of scan process
	int isfd[2];
	if (pipe(isfd) == -1) throw e_basic("can not create in pipe!!!");//e_basic???!!!
	int osfd[2];
	if (pipe(osfd) == -1) throw e_basic("can not create out pipe!!!");//e_basic???!!!

	// executing scan process with substituted fds
	vector<string> empty;
	pid_t cmdpid = utils::exec(module_smb__cmd_for_host.c_str(), empty, empty, isfd[0], osfd[1]);

	// creating streams to work with fds on our side
	close(isfd[0]); close(osfd[1]);
	fdostream is(isfd[1]);
	fdistream os(osfd[0]);

	// sending parameters to scanner process
	is <<        utils::inet_ntoa(request.address())	<< endl;
	is <<        request.share()		<< endl;
	is <<        request.username()		<< endl;
	is <<        request.password()		<< endl;
	is <<        request.workgroup()	<< endl;
	is <<        request.selfname()		<< endl;
	is << dec << request.timeout()		<< endl;

	// reading and handling output of scanner process
	do {
		string share = utils::getline(&os);
		os >> ws;
		c_path sharepath = utils::string2path(share);
		share = sharepath.empty()?"":sharepath[0];
		if (!os.eof() || (!share.empty()))
		{
//			cout << "we got share '" << share << "' on " << utils::inet_ntoa(request.address()) << endl; //!!!
			database->report_item(request, share);
		}
	} while (!os.eof());
	close(isfd[1]); close(osfd[0]);

	// closing process
	int wstat;
	waitpid(cmdpid, &wstat, 0);
	return wstat;
}

// options to scanner (per-process)
c_request  module_smb__request;

//
int module_smb ()
{
//	t_ipaddr  address = module_smb__address;
	c_request request = module_smb__request;
	int status_host = 0;
	if (request.share().empty())
	{
		status_host = smb_scan_host(request);
	} else {
	}
	return status_host;
}
