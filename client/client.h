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
#include "../server/server.h"

typedef struct client
{
	char addr_ip[45];
	int port;
} client;

typedef struct server_response
{
	int id;
	unsigned long time;
	int code;
	server * server_list;
}server_response;

void error(char *msg);

struct client * readFileRoot(char * filename);

struct server_response * parse_server(char * buffer);


