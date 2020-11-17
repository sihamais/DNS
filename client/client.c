/**
 * \file          client.c
 * \brief       Fonctionnalités coté client
 */
#include "client.h"

void error(char *msg)
{
    perror(msg);
    exit(0);
}
//#######################################################################

root_server *readFileRoot(char *filename)
{
    FILE *fd;
    char buff[1024];
    char delim1[2] = "|";

    // Allocations mémoire des structures
    root_server *rs_tab = malloc(sizeof(root_server));
    rs_tab->server_list = malloc(1000 * sizeof(server));
    rs_tab->size = 0;

    fd = fopen(filename, "r");

    while (fgets(buff, 100, fd) != NULL)
    {
        strcpy(rs_tab->server_list[rs_tab->size].addr_ip, strtok(buff, delim1));
        rs_tab->server_list[rs_tab->size].port = atoi(strtok(NULL, delim1));
        rs_tab->size++;
    }

    fclose(fd);
    return rs_tab;
}
//#######################################################################

char *request(char *ip, int port, int id, char *name)
{
    int sock;
    char *buffer = malloc(20000 * sizeof(char));
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
    int id = 1;
    char buffer[255];
    char *received;

    root_server *rs_tab;
    server_response *s1;
    server_response *s2;
    server_response *s3;

    rs_tab = readFileRoot(argv[1]);
    buffer[0] = '\0';

    if (argc == 2)
    {
        printf("Entrez le nom à résoudre : \n");
        scanf("%s", buffer);

        for (int i = 0; i < rs_tab->size; i++)
        {
            if ((received = request(rs_tab->server_list[i].addr_ip, rs_tab->server_list[i].port, id, buffer)) != NULL)
            {
                s1 = parse_server(received);
                for (int j = 0; j < s1->code; j++)
                {
                    if ((received = request(s1->server_list[j].addr_ip, s1->server_list[j].port, id + 1, buffer)) != NULL)
                    {
                        s2 = parse_server(received);
                        for (int k = 0; k < s2->code; k++)
                        {
                            if ((received = request(s2->server_list[k].addr_ip, s2->server_list[k].port, id + 2, buffer)) != NULL)
                            {
                                s3 = parse_server(received);
                                if (s3->code > 0)
                                {
                                    k = s2->code;
                                    j = s1->code;
                                    i = rs_tab->size;
                                    id += 3;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    // else if (argc == 3)
    // {
    //     FILE *fd;
    //     char buffer[1024];
    //     fd = fopen(argv[1], "r");

    //     while (fgets(buffer, 100, fd) != NULL)
    //     {
    //         for (int i = 0; i < rs_tab->size; i++)
    //         {
    //             if ((received = request(rs_tab->server_list[i].addr_ip, rs_tab->server_list[i].port, id, buffer)) != NULL)
    //             {
    //                 s1 = parse_server(received);
    //                 for (int j = 0; j < s1->code; j++)
    //                 {
    //                     if ((received = request(s1->server_list[j].addr_ip, s1->server_list[j].port, id + 1, buffer)) != NULL)
    //                     {
    //                         s2 = parse_server(received);
    //                         for (int k = 0; k < s2->code; k++)
    //                         {
    //                             if ((received = request(s2->server_list[k].addr_ip, s2->server_list[k].port, id + 2, buffer)) != NULL)
    //                             {
    //                                 s3 = parse_server(received);
    //                                 if (s3->code > 0)
    //                                 {
    //                                     k = s2->code;
    //                                     j = s1->code;
    //                                     i = rs_tab->size;
    //                                     id += 3;
    //                                 }
    //                             }
    //                         }
    //                     }
    //                 }
    //             }
    //         }
    //     }   
    //     fclose(fd);
    // }
    else
    {
        printf("Usage:\n    ./client <servers_file>\nor\n    ./client <servers_file> <names_file>\n");
        exit(1);
    }

    free(s1->server_list);
    free(s2->server_list);
    free(s3->server_list);
    free(s1);
    free(s2);
    free(s3);
    free(rs_tab);
    return 1;
}