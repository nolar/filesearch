CFLAGS=-Wall -I/usr/local/include -I/usr/local/include/mysql -O -pipe -mcpu=pentiumpro
LDFLAGS=-Wall -L/usr/local/lib -lsmbclient -L/usr/local/lib/mysql -lmysqlclient -lcrypt -lm  -lz

OBJECTS=		request.o fileinfo.o e_basic.o e_samba.o e_io.o options.o convert.o typedefs.o io.o
OBJECTS_FOR_CTL=	filesearch-ctl.o e_database.o database.o database_mysql.o e_fork.o forker.o thread.o thread_smb.o
OBJECTS_FOR_SMB=	filesearch-smb.o

all: filesearch-ctl filesearch-smb

clean:
	@rm -f *.o *.core *~ *.test.result *.test.debug *.test.log filesearch-ctl filesearch-smb

filesearch-ctl: ${OBJECTS} ${OBJECTS_FOR_CTL}
	g++ ${CFLAGS} ${LDFLAGS} $> -o $@

filesearch-smb: ${OBJECTS} ${OBJECTS_FOR_SMB}
	g++ ${CFLAGS} ${LDFLAGS} $> -o $@




AAA:  AAA.o c_object.o c_stream.o c_stopsign.o c_mapkey.o c_signed.o c_unsigned.o c_double.o c_flag.o c_stamp.o c_string.o
	g++ ${CFLAGS} ${LDFLAGS} $> -o $@

BBB: ${OBJECTS} BBB.o
	g++ ${CFLAGS} ${LDFLAGS} $> -o $@

