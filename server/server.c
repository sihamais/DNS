/**
 * @file          server.c
 * @brief       Fonctionnalités coté serveur
 */
#include "server.h"

void error(char *msg)
{
    perror(msg);
    exit(0);
}
//#######################################################################

server *readFileName(char *filename)
{
    int i = 0, j = 0;
    FILE *fd;
    char temp[1024];
    char temp2[1024];
    char buff[1024];
    char delim1[2] = "|";
    char delim2[2] = ".";

    struct server *s_tab;
    s_tab = malloc(1000 * sizeof(server));

    fd = fopen(filename, "r"); // Ouverture du fichier

    while (fgets(buff, 100, fd) != NULL) // Lecture du fichier ligne par ligne
    {
        j = 0;
        strcpy(s_tab[i].url, strtok(buff, delim1));     // Récupération du nom
        strcpy(s_tab[i].addr_ip, strtok(NULL, delim1)); // Récupération de l'adresse IP
        s_tab[i].port = atoi(strtok(NULL, delim1));     // Récupération du numéro de port

        strcpy(temp, s_tab[i].url);
        strcpy(temp2, s_tab[i].url);

        strtok(temp, delim2);
        while (strtok(NULL, delim2) != NULL)
        {
            j++;
        }

        if (j == 0)
        {
            strcpy(s_tab[i].domain, strtok(temp2, delim2)); // Récupération du nom de domaine
        }
        else if (j == 1)
        {
            strcpy(s_tab[i].child_domain, strtok(temp2, delim2)); // Récupération du nom de sous domaine
            strcpy(s_tab[i].domain, strtok(NULL, delim2));        // Récupération du nom de domaine
        }
        else
        {
            strcpy(s_tab[i].name, strtok(temp2, delim2));        // Récupération du nom de machine
            strcpy(s_tab[i].child_domain, strtok(NULL, delim2)); // Récupération du nom de sous domaine
            strcpy(s_tab[i].domain, strtok(NULL, delim2));       // Récupération du domaine
        }
        i++;
    }

    fclose(fd);
    return s_tab;
}
//#######################################################################

client_response *getresponse(server *s, client_response *cr)
{
    int j = 0;
    for (int i = 0; i < 1000 && s[i].port != 0; i++)
    {
        if (cr->id % 3 == 1 && strcmp(cr->domain, s[i].domain) == 0) // Si c'est la requête vers le serveur racine
        {
            // On stocke les serveurs correspondants à la requête client
            strcpy(cr->server_list[j].url, s[i].url);
            strcpy(cr->server_list[j].addr_ip, s[i].addr_ip);
            cr->server_list[j].port = s[i].port;
            j++;
        }
        else if (cr->id % 3 == 2 && strcmp(cr->child_domain, s[i].child_domain) == 0) // Si c'est la requête vers le serveur sous domaine
        {
            // On stocke les serveurs correspondants à la requête client
            strcpy(cr->server_list[j].url, s[i].url);
            strcpy(cr->server_list[j].addr_ip, s[i].addr_ip);
            cr->server_list[j].port = s[i].port;
            j++;
        }
        else if (cr->id % 3 == 0 && strcmp(cr->name, s[i].name) == 0) // Si c'est la requête vers le serveur nom
        {
            // On stocke les serveurs correspondants à la requête client
            strcpy(cr->server_list[j].url, s[i].url);
            strcpy(cr->server_list[j].addr_ip, s[i].addr_ip);
            cr->server_list[j].port = s[i].port;
            j++;
        }
    }

    cr->code = j; // On garde le nombre de résultats trouvés
    return cr;
}
//#######################################################################

client_response *parse_client(char *buffer)
{
    char temp[250];
    char delim1[2] = "|";
    char delim2[2] = ".";

    client_response *res;
    res = malloc(sizeof(client_response));
    res->server_list = malloc(100 * sizeof(server));

    res->id = atoi(strtok(buffer, delim1)); // Récupération de l'id de la requête client
    res->time = atol(strtok(NULL, delim1)); // Récupération de l'horodatage

    strcpy(temp, strtok(NULL, delim1));
    strcpy(res->buffer, temp);

    strcpy(res->name, strtok(temp, delim2));         // Récupération du nom
    strcpy(res->child_domain, strtok(NULL, delim2)); // Récupération du sous domaine
    strcpy(res->domain, strtok(NULL, delim2));       // Récupération du domaine

    return res;
}
//#######################################################################

void receive_send(int sock, server* s)
{
    socklen_t fromlen;
    int length;
    char buffer[1024];
    char resp[1024];
    char temp[1024];
    struct sockaddr_in from;
    client_response *cr;

    memset(buffer, 0, strlen(buffer));

    fromlen = sizeof(struct sockaddr_in);

    if ((length = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&from, &fromlen)) < 0) // Réception de la requête client
    {
        error("recvfrom failed");
    }
    printf("received : %s\n", buffer);

    buffer[length]= '\0';

    cr = parse_client(buffer); // Appel a la fonction parse_client pour parser la réponse du client
    cr = getresponse(s, cr); // Appel à la fonction getresponse pour filtrer les bonnes adresses à renvoyer

    snprintf(resp, 1024, "%d|%ld|%s|%d", cr->id, cr->time, cr->buffer, cr->code); // Création de la réponse à envoyer au client

    for (int i = 0; i < cr->code; i++)
    {
        snprintf(temp, 1024, "|%s,%s,%d", cr->server_list[i].url, cr->server_list[i].addr_ip, cr->server_list[i].port);
        strcat(resp, temp); // Concaténation de la liste des serveurs correspondants à la requête client
    }

    if (sendto(sock, resp, strlen(resp), 0, (struct sockaddr *)&from, fromlen) < 0) // Envoi de la réponse du serveur au client
    {
        error("sendto failed");
    }
    printf("sent : %s\n\n", resp);
}
//#######################################################################

int main(int argc, char **argv)
{
    int sock; 
    struct sockaddr_in server;
    struct server *s_tab;

    if (argc != 3)
    {
        printf("Usage:\n    ./server <port> <servers_file>\n");
        exit(1);
    }

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) // Initialisation du socket
    {
        error("Opening socket");
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1])); // Initialisation du port passé en paramètre
    server.sin_addr.s_addr= INADDR_ANY;

    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
        error("bind failed");

    s_tab = readFileName(argv[2]);      // Lecture du fichier serveur

    while (1)
    {
        receive_send(sock, s_tab);        // Réception de la requête client
    }

    free(s_tab);
    close(sock);
    return 1;
}