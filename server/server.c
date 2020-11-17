/**
 * \file          server.c
 * \brief       Fonctionnalités coté serveur
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

    fd = fopen(filename, "r");     // Ouverture du fichier


    while (fgets(buff, 100, fd) != NULL) // Lecture du fichier ligne par ligne
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

client_response *getresponse(server *s, client_response *cr)
{
    int j = 0;
    for (int i = 0; i < 1000 && s[i].port != 0; i++)
    {
        if (cr->id % 3 == 1 && strcmp(cr->domain, s[i].domain) == 0)
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

client_response *parse_client(char *buffer)
{
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

client_response *receive(int sock, int port)
{
    struct sockaddr_in server;
    socklen_t fromlen;
    int length;
    char buffer[1024];
    struct sockaddr_in from;
    client_response *cr;

    fromlen = sizeof(struct sockaddr_in);

    memset(&server, 0, sizeof(server));
    memset(buffer, 0, strlen(buffer));
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

void respond(int sock, client_response *cr)
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

    if (sendto(sock, resp, strlen(resp), 0, (struct sockaddr *)&from, fromlen) < 0)
    {
        error("sendto failed");
    }
    printf("sent : %s\n\n", resp);
}
//#######################################################################

int main(int argc, char **argv)
{
    int sock;
    struct server *s_tab;
    client_response *res;

    if (argc != 3)
    {
        printf("Usage:\n    ./server <port> <servers_file>\n");
        exit(1);
    }

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        error("Opening socket");
    }

    s_tab = readFileName(argv[2]);

    res = receive(sock, atoi(argv[1]));
    res = getresponse(s_tab, res);
    respond(sock, res);

    free(s_tab);
    free(res);
    close(sock);
    return 1;
}