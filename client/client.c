#include "client.h"

void error(char *msg)
{
    perror(msg);
    exit(0);
}
//#######################################################################

client *readFileRoot(char *filename)
{
    int i = 0;
    FILE *fd;
    char buff[1024];
    char delim1[2] = "|";

    client *c_tab;
    c_tab = malloc(1000 * sizeof(client));

    fd = fopen(filename, "r");

    while (fgets(buff, 100, fd) != NULL)
    {
        strcpy(c_tab[i].addr_ip, strtok(buff, delim1));
        c_tab[i].port = atoi(strtok(NULL, delim1));
        i++;
    }

    fclose(fd);
    return c_tab;
}
//#######################################################################

char *request(char *ip, int port, int id, char *name)
{
    int sock, length;
    char *buffer = malloc(20000 * sizeof(char));
    char *res;
    struct sockaddr_in server;
    struct sockaddr_in from;
    socklen_t fromlen;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 10000;

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, ip, &(server.sin_addr.s_addr));

    fromlen = sizeof(struct sockaddr_in);

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        error("socket");
    }

    snprintf(buffer, 2000, "%d|%ld|%s", id, time(NULL), name);

    if (sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        error("erreur");
    }

    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        return NULL;
    }

    printf("sent : %s\nto %d\n", buffer, port);

    if (recvfrom(sock, buffer, 1024, 0, (struct sockaddr *)&from, &fromlen) < 0)
    {
        return NULL;
    }

    buffer[strlen(buffer)] = '\0';
    printf("received : %s\n\n", buffer);

    close(sock);
    return buffer;
}
//#######################################################################

// Parsing de la réponse serveur
server_response *parse_server(char *buffer)
{
    int i, j;
    char delim1[2] = "|";
    char delim2[2] = ",";

    server_response *res;
    res = malloc(sizeof(server_response));

    res->id = atoi(strtok(buffer, delim1));
    res->time = atol(strtok(NULL, delim1));
    strtok(NULL, delim1);
    res->code = atoi(strtok(NULL, delim1));

    char *matches[res->code];
    for (i = 0; i < res->code; i++)
    {
        matches[i] = malloc(300 * sizeof(char));
    }

    res->server_list = malloc(res->code * sizeof(server));

    for (i = 0; i < res->code; i++)
    {
        strcpy(matches[i], strtok(NULL, delim1));
    }

    for (j = 0; j < res->code; j++)
    {
        strcpy(res->server_list[j].url, strtok(matches[j], delim2));
        strcpy(res->server_list[j].addr_ip, strtok(NULL, delim2));
        res->server_list[j].port = atoi(strtok(NULL, delim2));
        free(matches[j]);
    }

    free(buffer);
    return res;
}
//#######################################################################

int main(int argc, char **argv)
{
    int id = 1, index = 0,i=0;
    char buffer[255];
    char *received;

    server_response *res;
    client *c_tab;

    c_tab = readFileRoot(argv[1]);
    memset(buffer, 0, strlen(buffer));

    if (argc == 2)
    {
        strcpy(buffer, "www.sihamais.com");

        char *addr = c_tab[index].addr_ip;
        int port = c_tab[index].port;

        while(i<3)
        {
            received = request(addr, port, id, buffer);
            if (received != NULL)
            {
                res = parse_server(received);

                if (res->code > 0)
                {
                    id++;
                    i++;
                    index = 0;
                    addr = res->server_list[index].addr_ip;
                    port = res->server_list[index].port;
                }
                else
                {
                    index++;
                }
            }
            else
            {
                printf("ici\n");
                index++;
                printf("%s|", res->server_list[1].addr_ip);
                printf("%d",res->server_list[1].port);
            }
        }
    }
    else
    {
        printf("Usage:\n    ./client <servers_file>\nor\n    ./client <servers_file> <names_file>\n");
        exit(1);
    }

    free(c_tab);
    free(res->server_list);
    free(res);
    return 1;
}