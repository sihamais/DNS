#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <time.h>
#include <arpa/inet.h>
#include "../server/server.h"

typedef struct root_server
{
	int size;
	server * server_list;
} root_server;

typedef struct server_response
{
	int id;
	unsigned long time;
	int code;
	server * server_list;
}server_response;

void error(char *msg);

struct root_server * readFileRoot(char * filename);

char *request(char *ip, int port, int id, char *name);

struct server_response * parse_server(char * buffer);


