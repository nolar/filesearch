CFLAGS=-Wall -I/usr/local/include -I/usr/local/include/mysql -O -pipe -mcpu=pentiumpro
LDFLAGS=-Wall -L/usr/local/lib -lsmbclient -L/usr/local/lib/mysql -lmysqlclient -lcrypt -lm  -lz

OBJECTS=e_basic.o \
	c_object.o c_stopsign.o c_mapkey.o c_signed.o c_unsigned.o c_double.o c_flag.o c_stamp.o c_string.o c_ipaddr.o c_path.o c_action.o c_protocol.o \
	globals.o c_stream.o
OBJECTS_FOR_CTL=	filesearch-ctl.o e_database.o database.o database_mysql.o e_fork.o forker.o thread.o thread_smb.o request.o fileinfo.o 
OBJECTS_FOR_SMB=	filesearch-smb.o e_samba.o

all: filesearch-ctl filesearch-smb

clean:
	@rm -f *.o *.core *~ *.test.result *.test.debug *.test.log filesearch-ctl filesearch-smb

filesearch-ctl: ${OBJECTS} ${OBJECTS_FOR_CTL}
	g++ ${CFLAGS} ${LDFLAGS} $> -o $@

filesearch-smb: ${OBJECTS} ${OBJECTS_FOR_SMB}
	g++ ${CFLAGS} ${LDFLAGS} $> -o $@

filesearch-test-makesmbtask: ${OBJECTS} filesearch-test-makesmbtask.o
	g++ ${CFLAGS} ${LDFLAGS} $> -o $@




AAA:  ${OBJECTS} AAA.o
	g++ ${CFLAGS} ${LDFLAGS} $> -o $@
