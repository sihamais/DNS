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
	char addr_ip[100];
	int port;
} root_server;

typedef struct name_server
{
	char domain;
	char addr_ip;
	int port;
} name_server;

int main(int argc, char ** argv)
{
	int len, fd, i=0 ,j = 0;
	ssize_t n;
	char c;
	char * buff;
	
	struct root_server *rs_tab;
	rs_tab = malloc(1000 * sizeof(root_server));

	if (argc != 2)
	{
		perror("Usage : ./file_process <file_name>");
	}

	fd = open(argv[1], O_RDONLY);

	while ((n = read(fd, &c, 1)) > 0)
	{
		if (c == '|')
		{
			struct root_server rs;
			buff[j]='\0';
			strcpy(rs_tab[i].addr_ip, buff);
			printf("%s\n", rs_tab[i].addr_ip);
			j = 0;
		}
		else if (c == '\n' || c == EOF ){
			char temp[6];
			buff[j]='\0';
			strcpy(temp, buff);
			rs_tab[i].port = atoi(temp);
			printf("%d\n", rs_tab[i].port);
			j = 0;
			i++;
		}
		else {
			buff[j] = c;
			j++;
		}
	}

	close(fd);
	return 0;
}