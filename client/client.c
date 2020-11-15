#include "client.h"

void error(char *msg)
{
    perror(msg);
    exit(0);
}

struct client *readFileRoot(char *filename)
{
    int fd, i = 0, j = 0;
    ssize_t n;
    char c;
    char *buff;

    struct client *c_tab;
    c_tab = malloc(1000 * sizeof(client));

    fd = open(filename, O_RDONLY);

    while ((n = read(fd, &c, 1)) > 0)
    {
        if (c == '|')
        {
            buff[j] = '\0';
            strcpy(c_tab[i].addr_ip, buff);
            j = 0;
        }
        else if (c == '\n')
        {
            buff[j] = '\0';
            c_tab[i].port = atoi(buff);
            j = 0;
            i++;
        }
        else
        {
            buff[j] = c;
            j++;
        }
    }
    buff[j] = '\0';
    c_tab[i].port = atoi(buff);
    i++;

    close(fd);
    return c_tab;
}

char *request(int sock, struct sockaddr_in server, char *ip, int port, int id, char *name)
{
    char * buffer;
    char * res;
    int fromlen, length;
    struct sockaddr_in from;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(atoi(ip));

    fromlen = sizeof(struct sockaddr_in);

    snprintf(buffer, 1024, "%d|%ld|%s", id, time(NULL), name);

    if (sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        error("sendto failed");
    }

    if (recvfrom(sock, buffer, 256, 0, (struct sockaddr *)&from, &fromlen) < 0)
    {
        error("recvfrom failed");
    }

    printf("%s",buffer);

    return buffer;
}

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

int main(int argc, char **argv)
{
    int sock, id = 1, index = 0;
    char buffer[256];
    char *received;

    struct server_response *res;
    struct client *c_tab;
    struct sockaddr_in server;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        error("socket");
    }

    c_tab = readFileRoot(argv[1]);

    if (argc == 2)
    {
        printf("Entrez votre requête : ");
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);

        char *addr = c_tab[index].addr_ip;
        int port = c_tab[index].port;

        while (id < 3)
        {
            received = request(sock, server, addr, port, id, buffer);
            res = parse_server(received);

            if (res->code <= 0)
            {
                index++;
                continue;
            }
            else if (res->code == 0)
            {
                index++;
                continue;
            }
            else
            {
                id++;
                index = 0;
                addr = res->server_list[index].addr_ip;
                port = res->server_list[index].port;
            }
        }
    }
    else if (argc == 3)
    {
    }
    else
    {
        printf("Usage:\n    ./client <servers_file>\nor\n    ./client <server_file> <names_file>\n");
        exit(1);
    }

    printf("Liste de serveurs racines:\n");
    for (int i = 0; i < 1000 && c_tab[i].port != 0; i++)
    {
        printf("\n serveur %d :\n    adresse ip : %s\n    port : %d\n\n", i, c_tab[i].addr_ip, c_tab[i].port);
    }

    free(c_tab);
    close(sock);
    return 1;
}