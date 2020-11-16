#include "client.h"

void error(char *msg)
{
    perror(msg);
    exit(0);
}
//#######################################################################

struct client *readFileRoot(char *filename)
{
    int i = 0;
    FILE *fd;
    char buff[1024];
    char delim1[2] = "|";

    struct client *c_tab;
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
    int sock;
    struct sockaddr_in server;
    char *buffer;
    char *res;
    socklen_t fromlen;
    int length;
    struct sockaddr_in from;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        error("socket");
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, ip, &(server.sin_addr));

    fromlen = sizeof(struct sockaddr_in);

    snprintf(buffer, 1024, "%d|%ld|%s", id, time(NULL), name);

    if (sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        error("sendto failed");
    }

    printf("sent :  %s\n", buffer);

    if (recvfrom(sock, buffer, 1024, 0, (struct sockaddr *)&from, &fromlen) < 0)
    {
        error("recvfrom failed");
    }

    printf("received : %s\n", buffer);
    close(sock);

    return buffer;
}
//#######################################################################

// Parsing de la réponse serveur
struct server_response *parse_server(char *buffer)
{
    char *token;
    char delim1[2] = "|";
    char delim2[2] = ",";

    struct server_response *res;
    res = malloc(sizeof(struct server_response));

    res->id = atoi(strtok(buffer, delim1));
    res->time = atol(strtok(NULL, delim1));
    res->code = atoi(strtok(NULL, delim1));

    res->server_list = malloc(res->code * sizeof(server));

    for (int i = 0; i < res->code; i++)
    {
        token = strtok(NULL, delim1);
        strcpy(res->server_list[i].domain, strtok(NULL, delim2));
        strcpy(res->server_list[i].addr_ip, strtok(NULL, delim2));
        res->server_list[i].port = atoi(strtok(NULL, delim2));
    }

    return res;
}
//#######################################################################

int main(int argc, char **argv)
{
    int id = 1, index = 0, i = 0;
    char buffer[255];
    char * received;

    struct server_response *res;
    struct client *c_tab;

    c_tab = readFileRoot(argv[1]);

    if (argc == 2)
    {
        strcpy(buffer, "www.sihamais.com");

        char *addr = c_tab[index].addr_ip;
        int port = c_tab[index].port;

        while (i < 3)
        {
            received = request(addr, port, id, buffer);
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
                continue;
            }
        }
    }
    else
    {
        printf("Usage:\n    ./client <servers_file>\nor\n    ./client <servers_file> <names_file>\n");
        exit(1);
    }

    free(c_tab);

    return 1;
}