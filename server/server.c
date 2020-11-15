#include "server.h"

// Traitement d'erreur
void error(char *msg)
{
    perror(msg);
    exit(0);
}
//#######################################################################

// Lecture du fichier des serveurs de noms
struct server *readFileName(char * filename)
{
    int i = 0, j = 1;
    FILE *fd;
    char *token;
    char * temp;
    char *buff;
    char delim1[2] = "|";
    char delim2[2] = ".";

    struct server *s_tab;
    s_tab = malloc(1000 * sizeof(server));

    if ((fd = fopen(filename, "r")) == NULL)
    {
        error("fopen");
    }

    while (fgets(buff, 100, fd) != NULL)
    {
        token = strtok(buff, delim1);
        temp = strtok(buff,delim1);
        strcpy(s_tab[i].url,token);

        while(strtok(temp,delim2)!=NULL){
            j++;
        }

        if(j == 2){
            if (strcmp(strtok(token,delim2),"")==0){
                strcpy(s_tab[i].domain, strtok(token,delim2));
            }
            else
            {
                strcpy(s_tab[i].child_domain, strtok(token,delim2));
                strcpy(s_tab[i].domain, strtok(NULL,delim2));
            }
        }
        else{
                strcpy(s_tab[i].name, strtok(token,delim2));
                strcpy(s_tab[i].child_domain, strtok(NULL,delim2));
                strcpy(s_tab[i].domain, strtok(NULL,delim2));
        }
        strcpy(s_tab[i].addr_ip,strtok(NULL,delim1));
        s_tab[i].port = atoi(strtok(NULL,delim1));
        i++;
    }

    printf("Liste de serveurs noms:\n");
    for (int i = 0; i < 1000 && s_tab[i].port != 0; i++)
    {
        printf("\n serveur %d:\n    domaine : %s\n    adresse ip : %s\n    port : %d\n\n", i + 1, s_tab[i].url, s_tab[i].addr_ip, s_tab[i].port);
    }

    fclose(fd);
    return s_tab;
}
//#######################################################################

// Filtrage de la liste des serveurs corresspondant à la requête client
/*
completer les differents cas d'id
*/
struct client_response *getresponse(struct server *s, struct client_response *cr)
{
    int j = 0;

    for (int i = 0; i < 1000 && s[i].port != 0; i++)
    {
        if (cr->id % 3 == 1 && strcmp(cr->domain, s[i].domain) == 0)
        {
            strcpy(cr->server_list[j].domain, s[i].domain);
            strcpy(cr->server_list[j].addr_ip, s[i].addr_ip);
            cr->server_list[j].port = s[i].port;
            j++;
        }
        else if (cr->id % 3 == 2 && strcmp(cr->child_domain,s[i].child_domain)== 0)
        {
            strcpy(cr->server_list[j].domain, s[i].domain);
            strcpy(cr->server_list[j].child_domain, s[i].child_domain);
            strcpy(cr->server_list[j].addr_ip, s[i].addr_ip);
            cr->server_list[j].port = s[i].port;
            j++;
        }
        else if(cr->id % 3 == 0 && strcmp(cr->name,s[i].name)== 0)
        {  
            strcpy(cr->server_list[j].name, s[i].name);
            strcpy(cr->server_list[j].domain, s[i].domain);
            strcpy(cr->server_list[j].child_domain, s[i].child_domain);
            strcpy(cr->server_list[j].addr_ip, s[i].addr_ip);
            cr->server_list[j].port = s[i].port;
            j++;
        }
    }

    cr->code = j;
    return cr;
}
//#######################################################################

// Parsing de la requete client
struct client_response *parse_client(char *buffer)
{
    char *token;
    char delim1[2] = "|";
    char delim2[2] = ",";

    struct client_response *res;
    res = malloc(sizeof(struct client_response));
    res->server_list = malloc(1000 * sizeof(struct server));
    strcpy(res->buffer, buffer);
    res->id = atoi(strtok(buffer, delim1));
    res->time = atol(strtok(NULL, delim1));
    token = strtok(NULL, delim1);
    strtok(NULL, delim2);
    strcpy(res->name, strtok(NULL, delim2));
    strcpy(res->child_domain, strtok(NULL, delim2));
    strcpy(res->domain, strtok(NULL, delim2));

    return res;
}
//#######################################################################

// Reception de la requete client
struct client_response *receive(int sock, struct sockaddr_in server, int port)
{
    int fromlen, length;
    struct sockaddr_in from;
    struct client_response *cr;

    fromlen = sizeof(struct sockaddr_in);

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        error("bind failed");
    }

    char buffer[1024];
    if ((length = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&from, &fromlen)) < 0)
    {
        error("recvfrom failed");
    }
    buffer[length] = '\0';
    printf("%s\n", buffer);

    cr = parse_client(buffer);
    cr->from = &from;
    return cr;
}
//#######################################################################

// Réponse du server avec les sockets
void respond(int sock, struct sockaddr_in server, struct client_response *cr)
{
    int fromlen;
    struct sockaddr_in *from;
    from = cr->from;
    char *resp;
    char *temp;

    fromlen = sizeof(struct sockaddr_in);

    snprintf(resp, 1024, "%d|%ld|%s|%d", cr->id, cr->time, cr->buffer, cr->code);

    for (int i = 0; i < cr->code + 1; i++)
    {
        snprintf(temp, 1024, "|%s,%s,%d", cr->server_list[i].domain, cr->server_list[i].addr_ip, cr->server_list[i].port);
        strcat(resp, temp);
    }
    strcat(resp, "\n");

    if (sendto(sock, resp, sizeof(resp), 0, (struct sockaddr *)&from, fromlen) < 0)
    {
        error("sendto failed");
    }
    printf("sendto ok");
}
//#######################################################################

// Main du programme
int main(int argc, char **argv)
{
    // int sock;
    // struct sockaddr_in server;
    struct server *s_tab;
    // struct client_response *res;

    // if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    // {
    //     error("Opening socket");
    // }

    // res = receive(sock, server, atoi(argv[1]));
    s_tab = readFileName("server.txt");
    // res = getresponse(s_tab, res);
    // respond(sock, server, res);

    free(s_tab);
    // free(res->server_list);
    // free(res);
    // close(sock);
    return 1;
}