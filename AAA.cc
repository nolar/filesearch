#include "c_stream.h"
#include "c_object.h"
#include "c_signed.h"
#include "c_unsigned.h"
#include "c_double.h"
#include "c_flag.h"
#include "c_stamp.h"
#include "c_string.h"

#include <time.h>
#include <fcntl.h>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <exception>

using namespace std;

int main ()
{
	try{
		int fd = open("DATA", O_RDWR|O_CREAT);
		if (fd == -1) cout << "Error:" << strerror(errno);
		else {
			c_stream s(fd);

/*			do {
				c_object * o = s.read_object();
				cout << c_stream::object_name(o) << endl;
				if (c_stream::object_typeof(o) == c_stream::ot_string) cout << "STRING:" << dynamic_cast<c_string*>(o)->get() << endl;
				if (c_stream::object_typeof(o) == c_stream::ot_stamp ) cout << "STAMP :" << dynamic_cast<c_stamp *>(o)->unixtime() << endl;
			} while (!s.status());*/

/*			std::vector<c_object*> v;
			v.push_back(new c_unsigned(123));
			v.push_back(new c_flag(true));
			v.push_back(new c_string("Hello world"));
			v.push_back(new c_flag(false));
			v.push_back(new c_unsigned(789));
			v.push_back(new c_stamp(time(NULL)));
			s.write_vector(v);*/

/*			std::map<c_mapkey,c_object*> m;
			m[ 2] = new c_unsigned(123);
			m[10] = new c_string("Hello world");
			m[33] = new c_stamp(time(NULL));
			s.write_map(m);*/

			std::map<c_mapkey,c_object*> m = s.read_map();
			cout << "COUNT=" << m.size() << endl;
			cout << "KEYS:";
			for (std::map<c_mapkey,c_object*>::const_iterator i = m.begin(); i!=m.end(); i++)
				cout <<" " << i->first.get();
			cout << endl;
			c_string * v = dynamic_cast<c_string*>(m[10]);
			cout << "S=" << (v?v->get():"NULL") << endl;
		}
		close(fd);
	}
	catch (std::exception &e)
	{
		cout << "ES: " << e.what() << endl;
	}
}

