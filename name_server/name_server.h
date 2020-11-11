#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

typedef struct name_server
{
	char domain[100];
	char addr_ip[45];
	int port;
} name_server;

struct name_server * readFileName(char * filename);