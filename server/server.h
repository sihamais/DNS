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

typedef struct server
{
	char domain[100];
	char addr_ip[45];
	int port;
} server;

typedef struct client_response{
	int id;
	unsigned long time;
	char * name_server;
	char * child_domain;
	char * domain;
}client_response;

void error(char *msg);

struct server * readFileName(char * filename);

void respond(int sock, struct sockaddr_in server, int port);

struct client_response * parse_client(char * buffer);

