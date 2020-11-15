#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

void error(char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char const *argv[])
{
	int sock, length, fromlen, n;
	struct sockaddr_in server;
	struct sockaddr_in from;
	char buf[1024];

	if (argc < 2)
	{
		fprintf(stderr, "ERROR , aucun port fourni\n");
		exit(0);
	}

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	
	if (sock < 0)
	{
		error("Opening socket");
	}
	length = sizeof(server);
	bzero(&server, length);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(atoi(argv[1]));

	if (bind(sock, (struct sockaddr *)&server, length) < 0)
	{
		error("binding");
	}

	fromlen = sizeof(struct sockaddr_in);

	while (1)
	{
		if((n = recvfrom(sock, buf, 1024, 0, (struct sockaddr *)&from, &fromlen))<0)
		{
			error("recvfrom");
		}

		write(1, "message reçu: ", 21);
		write(1, buf, n);
		sleep(3);

		if ((n = sendto(sock, "J'ai reçu votre message\n", 50, 0, (struct sockaddr *)&from, fromlen)) < 0)
		{
			error("sendto");
		}
	}
}