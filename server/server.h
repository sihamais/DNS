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
	char url[100];
	char name[100];
	char child_domain[100];
	char domain[100];
	char addr_ip[45];
	int port;
} server;

typedef struct client_response{
	int id;
	int code;
	unsigned long time;
	char buffer[250];
	char name[100];
	char child_domain[100];
	char domain[100];
	struct sockaddr_in  from;
	server * server_list;
}client_response;

// Traitement d'erreur
void error(char *msg);

// Lecture du fichier des serveurs de noms
server * readFileName(char * filename);

// Reception de la requete client
client_response *receive(int sock, struct sockaddr_in server, int port);

// Parsing de la requete client
client_response *parse_client(char *buffer);

// Filtrage de la liste des serveurs corresspondant à la requête client
client_response *getresponse(server *s, client_response *cr);

// Réponse du server avec les sockets
void respond(int sock, struct sockaddr_in server, client_response *cr);


