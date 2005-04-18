CFLAGS=-Wall -I/usr/local/include -I/usr/local/include/mysql -O -pipe -mcpu=pentiumpro -D_THREAD_SAFE
LDFLAGS=-Wall -L/usr/local/lib -lsmbclient -L/usr/local/lib/mysql -lmysqlclient_r -lcrypt -lm  -lz

OBJECTS_FOR_SHARE=	scan-share.o utils.o request.o e_basic.o e_samba.o e_iconv.o options.o
OBJECTS_FOR_HOST=	scan-host.o  utils.o request.o e_basic.o e_samba.o e_iconv.o options.o e_database.o database.o database_mysql.o
OBJECTS_FOR_SCANNER=	scanner.o    utils.o request.o e_basic.o e_samba.o e_iconv.o options.o e_database.o database.o database_mysql.o e_fork.o forker.o module_smb.o

all: scan-share scan-host scanner

scan-share: ${OBJECTS_FOR_SHARE}
	g++ ${CFLAGS} ${LDFLAGS} ${OBJECTS_FOR_SHARE} -o $@

scan-host: ${OBJECTS_FOR_HOST}
	g++ ${CFLAGS} ${LDFLAGS} ${OBJECTS_FOR_HOST} -o $@

scanner: ${OBJECTS_FOR_SCANNER}
	g++ ${CFLAGS} ${LDFLAGS} ${OBJECTS_FOR_SCANNER} -o $@

clean:
	@rm -f *.o *~ *.result *.errors scanner scan-host scan-share
