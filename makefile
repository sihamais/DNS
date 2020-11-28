CC=gcc
CFLAGS=-Wextra -Wall

all: clean serverprog clientprog
	@echo Compiled successfully !

serverprog: server/server.c server/server.h
	$(CC) server/server.c -o server/serv $(CFLAGS)

clientprog: client/client.c client/client.h
	$(CC) client/client.c -o client/cli $(CFLAGS)

clean:
	rm -rf server/serv client/cli latex html latex doc.html saissaoui-mahras.tar.gz rtf man man3

dist :  clean
	tar -zcvf saissaoui-mahras.tar.gz client/* server/* makefile Rapport.pdf Doxyfile script.sh

doc: clean
	doxygen
	ln -s ./html/index.html ./doc.html