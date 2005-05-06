CFLAGS=-Wall -I/usr/local/include -I/usr/local/include/mysql -O -pipe -mcpu=pentiumpro
LDFLAGS=-Wall -L/usr/local/lib -lsmbclient -L/usr/local/lib/mysql -lmysqlclient -lcrypt -lm  -lz

OBJECTS=		utils.o request.o fileinfo.o e_basic.o e_samba.o e_stream.o options.o
OBJECTS_FOR_SMB=	filesearch-smb.o
OBJECTS_FOR_MAIN=	filesearch.o e_database.o database.o database_mysql.o e_fork.o forker.o thread.o thread_smb.o

all: filesearch-smb filesearch

clean:
	@rm -f *.o *.core *~ *.test.result *.test.errors scanner filesearch-smb

filesearch-smb: ${OBJECTS} ${OBJECTS_FOR_SMB}
	g++ ${CFLAGS} ${LDFLAGS} $> -o $@

filesearch: ${OBJECTS} ${OBJECTS_FOR_MAIN}
	g++ ${CFLAGS} ${LDFLAGS} $> -o $@

AAA: ${OBJECTS} AAA.o
	g++ ${CFLAGS} ${LDFLAGS} $> -o $@

BBB: ${OBJECTS} BBB.o
	g++ ${CFLAGS} ${LDFLAGS} $> -o $@

