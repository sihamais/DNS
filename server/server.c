#include "server.h"

// Traitement d'erreur
void error(char *msg)
{
    perror(msg);
    exit(0);
}
//#######################################################################

// Lecture du fichier des serveurs de noms
server *readFileName(char *filename)
{
    int i = 0, j = 0;
    FILE *fd;
    char *token;
    char temp[1024];
    char temp2[1024];
    char buff[1024];
    char delim1[2] = "|";
    char delim2[2] = ".";

    struct server *s_tab;
    s_tab = malloc(1000 * sizeof(server));

    fd = fopen(filename, "r");

    while (fgets(buff, 100, fd) != NULL)
    {
        j = 0;
        strcpy(s_tab[i].url, strtok(buff, delim1));
        strcpy(s_tab[i].addr_ip, strtok(NULL, delim1));
        s_tab[i].port = atoi(strtok(NULL, delim1));

        strcpy(temp, s_tab[i].url);
        strcpy(temp2, s_tab[i].url);

        strtok(temp, delim2);
        while (strtok(NULL, delim2) != NULL)
        {
            j++;
        }

        if (j == 0)
        {
            strcpy(s_tab[i].domain, strtok(temp2, delim2));
        }
        else if (j == 1)
        {
            strcpy(s_tab[i].child_domain, strtok(temp2, delim2));
            strcpy(s_tab[i].domain, strtok(NULL, delim2));
        }
        else
        {
            strcpy(s_tab[i].name, strtok(temp2, delim2));
            strcpy(s_tab[i].child_domain, strtok(NULL, delim2));
            strcpy(s_tab[i].domain, strtok(NULL, delim2));
        }
        i++;
    }

    fclose(fd);
    return s_tab;
}
//#######################################################################

// Filtrage de la liste des serveurs corresspondant à la requête client
client_response *getresponse(server *s, client_response *cr)
{
    int j = 0;
    for (int i = 0; i < 1000 && s[i].port != 0; i++)
    {
        printf("%d\n", strcmp(cr->domain, s[i].domain));
        printf("%s\n", cr->domain);
        printf("%s\n", s[i].domain);

        if (cr->id % 3 == 1 && strcmp(cr->domain, s[i].domain) == 10)
        {
            strcpy(cr->server_list[j].url, s[i].url);
            strcpy(cr->server_list[j].addr_ip, s[i].addr_ip);
            cr->server_list[j].port = s[i].port;
            j++;
        }
        else if (cr->id % 3 == 2 && strcmp(cr->child_domain, s[i].child_domain) == 0)
        {
            strcpy(cr->server_list[j].url, s[i].url);
            strcpy(cr->server_list[j].addr_ip, s[i].addr_ip);
            cr->server_list[j].port = s[i].port;
            j++;
        }
        else if (cr->id % 3 == 0 && strcmp(cr->name, s[i].name) == 0)
        {
            strcpy(cr->server_list[j].url, s[i].url);
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
client_response *parse_client(char *buffer)
{
    char *token;
    char temp[250];
    char delim1[2] = "|";
    char delim2[2] = ".";

    client_response *res;
    res = malloc(sizeof(client_response));
    res->server_list = malloc(100 * sizeof(server));

    res->id = atoi(strtok(buffer, delim1));
    res->time = atol(strtok(NULL, delim1));

    strcpy(temp, strtok(NULL, delim1));
    strcpy(res->buffer, temp);

    strcpy(res->name, strtok(temp, delim2));
    strcpy(res->child_domain, strtok(NULL, delim2));
    strcpy(res->domain, strtok(NULL, delim2));

    return res;
}
//#######################################################################

// Reception de la requete client
client_response *receive(int sock, struct sockaddr_in server, int port)
{
    socklen_t fromlen;
    int length;
    char buffer[1024];
    struct sockaddr_in from;
    client_response *cr;

    fromlen = sizeof(struct sockaddr_in);

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        error("bind failed");
    }

    if ((length = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&from, &fromlen)) < 0)
    {
        error("recvfrom failed");
    }
    printf("received : %s\n", buffer);

    cr = parse_client(buffer);
    cr->from = from;
    return cr;
}
//#######################################################################

// Réponse du server avec les sockets
void respond(int sock, struct sockaddr_in server, client_response *cr)
{
    socklen_t fromlen;
    struct sockaddr_in from;
    from = cr->from;
    char resp[1024];
    char temp[1024];

    fromlen = sizeof(struct sockaddr_in);
    snprintf(resp, 1024, "%d|%ld|%s|%d", cr->id, cr->time, cr->buffer, cr->code);

    for (int i = 0; i < cr->code; i++)
    {
        snprintf(temp, 1024, "|%s,%s,%d", cr->server_list[i].url, cr->server_list[i].addr_ip, cr->server_list[i].port);
        strcat(resp, temp);
    }
    strcat(resp, "\n");

    if (sendto(sock, resp, strlen(resp), 0, (struct sockaddr *)&from, fromlen) < 0)
    {
        error("sendto failed");
    }
    printf("sent : %s\n", resp);
}
//#######################################################################

// Main du programme
int main(int argc, char **argv)
{
    int sock;
    struct sockaddr_in server;
    struct server *s_tab;
    client_response *res;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        error("Opening socket");
    }

    s_tab = readFileName(argv[2]);
    res = receive(sock, server, atoi(argv[1]));
    res = getresponse(s_tab, res);
    respond(sock, server, res);

    free(s_tab);
    free(res);
    close(sock);
    return 1;
}