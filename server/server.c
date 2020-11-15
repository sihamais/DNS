#include "server.h"

void error(char *msg)
{
    perror(msg);
    exit(0);
}

struct server *readFileName(char *filename)
{
    int fd, i = 0, j = 0;
    ssize_t n;
    char c;
    char *buff;

    struct server *s_tab;
    s_tab = malloc(1000 * sizeof(server));

    fd = open(filename, O_RDONLY);

    while ((n = read(fd, &c, 1)) > 0)
    {
        if (c == '|')
        {
            buff[j] = '\0';
            if (isdigit(buff[0]))
            {
                strcpy(s_tab[i].addr_ip, buff);
            }
            else
            {
                strcpy(s_tab[i].domain, buff);
            }
            j = 0;
        }
        else if (c == '\n')
        {
            buff[j] = '\0';
            s_tab[i].port = atoi(buff);
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
    s_tab[i].port = atoi(buff);
    i++;

    close(fd);
    return s_tab;
}

struct client_response *parse_client(char *buffer)
{
    char *token;
    char delim1[2] = "|";
    char delim2[2] = ",";

    struct client_response *res;
    res = malloc(sizeof(struct client_response));

    res->id = atoi(strtok(buffer, delim1));
    res->time = atol(strtok(NULL, delim1));
    token = strtok(NULL, delim1);
    strtok(NULL, delim2);
    strcpy(res->name_server, strtok(NULL, delim2));
    strcpy(res->child_domain, strtok(NULL, delim2));
    strcpy(res->domain, strtok(NULL, delim2));

    return res;
}

void respond(int sock, struct sockaddr_in server, int port)
{
    int fromlen, length, code;
    char * resp;
    struct sockaddr_in from;
    struct client_response * cr;

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        error("bind failed");
    }

    fromlen = sizeof(struct sockaddr_in);

    char buffer[1024];
    if((length = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&from, &fromlen)) < 0)
    {
        error("recvfrom failed");
    }
    buffer[length] = '\0';
    printf("%s\n", buffer);

    cr = parse_client(buffer);
    snprintf(resp,1024,"%d|%ld|%s|%d|%s|‰s|%d", cr->id, cr->time, buffer, code, cr->domain, //ip, //port);

    if (sendto(sock, "message received\n", 50, 0, (struct sockaddr *)&from, fromlen) < 0)
    {
        error("sendto failed");
    }
    printf("sendto ok");
}

int main(int argc, char **argv)
{
    int sock;
    struct sockaddr_in server;
    struct server *s_tab;
    struct client_response *res;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        error("Opening socket");
    }

    respond(sock, server, atoi(argv[1]));

    s_tab = readFileName("server.txt");

    printf("Liste de serveurs noms:\n");
    for (int i = 0; i < 1000 && s_tab[i].port != 0; i++)
    {
        printf("\n serveur %d:\n    domaine : %s\n    adresse ip : %s\n    port : %d\n\n", i + 1, s_tab[i].domain, s_tab[i].addr_ip, s_tab[i].port);
    }

    free(s_tab);
    close(sock);
    return 1;
}