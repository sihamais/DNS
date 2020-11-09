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

int main(int argc, char *argv[])
{
	int len, fd, i = 0;
	ssize_t n;
	char *buff;
	char *occur;

	struct root_server *rs_tab;
	rs_tab = malloc(1000 * sizeof(root_server));

	if (argc != 2)
	{
		perror("Usage : ./file_process <file_name>");
	}

	fd = open(argv[1], O_RDONLY);

	len = lseek(fd, 0, SEEK_END);
	lseek(fd, SEEK_SET, 0);
	printf("file size : %d", len);

	while ((n = read(fd, &buff, 1)) > 0)
	{
		if (buff[n] == '|' )
		{
			struct root_server rs;
			memcpy(rs_tab[i].addr_ip, buff, n);
			lseek(fd, SEEK_CUR, 1);
		}
		if (buff[n] == '\n'){
			char temp[6];
			memcpy(temp, buff, n);
			rs_tab[i].port = atoi(temp);
			lseek(fd, SEEK_CUR, 1);
			i++;
		}
	}

	// while (buff[0] != '\0')
	// {
	// 	struct root_server rs;
	// 	occur = strchr(buff, '|');
	// 	occur[0] = '\0';
	// 	memcpy(rs_tab[i].addr_ip, buff, occur - buff + 1);
	// 	buff = occur + 1;
	// 	occur = strchr(buff, '\n');
	// 	occur[0] = '\0';
	// 	memcpy(temp, buff, occur - buff + 1);
	// 	rs_tab[i].port = atoi(temp);
	// 	buff = occur + 1;
	// 	i++;
	// }
	close(fd);
	return 0;
}