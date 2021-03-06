/**
 * @file          client.h
 * @brief       Prototypes de fonctionnalités coté client
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
#include <arpa/inet.h>
#include <sys/time.h>
#include "../server/server.h"

/**
 * @brief  Structure pour stocker les informations des serveurs racines
 * 
 */
typedef struct root_server
{
	int size;
	server * server_list;
} root_server;

/**
 * @brief  Structure pour stocker la réponse du serveur reçue
 * 
 */
typedef struct server_response
{
	int id;
	unsigned long time;
	int code;
	server * server_list;
}server_response, sr;

/**
 * @brief Traitement d'erreur
 * 
 * @param msg 
 */
void error(char *msg);

/**
 * @brief Lecture du fichier des serveurs racines
 * 
 * @param filename 
 * @return struct root_server* 
 */
struct root_server * readFileRoot(char * filename);

/**
 * @brief Envoi de la requête client vers le serveur et réception de la réponse
 * 
 * @param ip
 * @param port 
 * @param id 
 * @param name 
 * @return char* 
 */
char *request(int sock, char *ip, int port, int id, char *name);

/**
 * @brief Parsing de la réponse du serveur pour remplir la structure de réponse
 * 
 * @param buffer 
 * @return struct server_response* 
 */
struct server_response * parse_server(char * buffer);

/**
 * @brief  Lance le traitement des requêtes. Cette fonction regroupe les autres fonctions.
 * 
 * @param sock
 * @param rs_tab
 * @param buffer
 * @param id
 */
void launch(int sock, root_server *rs_tab, sr *s1, sr * s2, sr * s3, char *buffer, int id);
