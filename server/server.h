/*! \mainpage DNS Projet Algores
 *
 * \section intro_sec How to
 *
 * Classes pour voir la documentation des structures
 * 
 * Files pour voir les fonctions de chaque fichier
 *
 */

/**
 * @file          server.h
 * @brief       Prototypes de fonctionnalités coté serveur
 */

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

/**
 * @brief Structure pour stocker les informations des serveurs
 * 
 */
typedef struct server
{
	char url[100];
	char name[100];
	char child_domain[100];
	char domain[100];
	char addr_ip[45];
	int port;
	int on;
} server;

/**
 * @brief Structure pour stocker la requête du client
 * 
 */
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

/**
 * @brief Traitement d'erreur
 * 
 * @param msg Message à afficher en cas d'erreur
 */
void error(char *msg);

/**
 * @brief Lecture du fichier des serveurs de noms
 * 
 * @param filename Nom de fichier à lire
 * @return server* 
 */
server * readFileName(char * filename);

/**
 * @brief Reception de la requete client
 * 
 * @param sock Descripteur de socket
 * @return client_response* 
 */
client_response *receive(int sock);

/**
 * @brief Parsing de la requete client pour pouvoir la stocker dans la structue client_response
 * 
 * @param buffer Tableau de char contenant la requête client
 * @return client_response* 
 */
client_response *parse_client(char *buffer);

/**
 * @brief Filtrage de la liste des serveurs corresspondant à la requête client
 * 
 * @param s 
 * @param cr 
 * @return client_response* 
 */
client_response *getresponse(server *s, client_response *cr);

/**
 * @brief Réponse du server avec les sockets
 * 
 * @param sock 
 * @param cr 
 */
void respond(int sock, client_response *cr);


