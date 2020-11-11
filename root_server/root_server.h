#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

typedef struct root_server
{
	char addr_ip[45];
	int port;
} root_server;

struct root_server * readFileRoot(char * filename);