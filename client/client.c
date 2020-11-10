#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

void error(char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sock , length, n;
	struct sockaddr_in server, from;
	struct hostent *hp;
	char buffer[256];

	if (argc != 3)
	{
		printf("Usage: server port\n");
		exit(1);
	}

	sock= socket(AF_INET, SOCK_DGRAM, 0);

	if (sock < 0)
	{
		error("socket");
	}

	server.sin_family = AF_INET;
	hp = gethostbyname(argv[1]);

	if (hp==0)
	{
		error("host inconnu");
	}

	bcopy((char *)hp->h_addr, (char *)&server.sin_addr,hp->h_length);
	server.sin_port = htons(atoi(argv[2]));
	length=sizeof(struct sockaddr_in);

	printf("Entrez un message: ");

	bzero(buffer , 256);
	fgets(buffer,255,stdin);
	n = sendto(sock,buffer,strlen(buffer),0,&server,length);
	if (n < 0)
	{
		error("sendto");
	}
	n=recvfrom(sock,buffer,256,0, (struct sockaddr *)&from, &length);
	if (n < 0)
	{
		error("recvfrom");
	}
	write(1,"GOT AN ACK: ",12);
	write(1,buffer,n);
}