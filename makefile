CC = gcc

all: 
	dns client.o root.o name.o

dns : client.o root.o name.o
	$(CC) -o dns client.o root.o name.o

client.o : client/client.c client/client.h
	$(CC) -o client.o -c client/client.c 

root.o : root_server/root_server.c root_server/root_server.h
	$(CC) -o root.o -c root_server/root_server.c 

name.o : name_server/name_server.c name_server/name_server.h
	$(CC) -o name.o -c name_server/name_server.c 	

dist : 
	tar -zcvf dns_resolution-AISSAOUI_MAHRAS.tar.xz client/* root_server/* name_server/* Makefile 

clean:
	@$(RM) -f dns *.o *.tar.xz client/*.out client/*.o root_server/*.out root_server/*.o name_server/*.out name_server/*.o
	@echo Clean!