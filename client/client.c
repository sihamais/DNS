/**
 * @file          client.c
 * @brief       Fonctionnalités coté client
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

    root_server *rs_tab = malloc(sizeof(root_server));   // Allocations mémoire des structures
    rs_tab->server_list = malloc(1000 * sizeof(server)); // Allocations mémoire des structures
    rs_tab->size = 0;

    fd = fopen(filename, "r"); // Ouverture du fichier

    while (fgets(buff, 100, fd) != NULL) // Lecture ligne par ligne du fichier
    {
        strcpy(rs_tab->server_list[rs_tab->size].addr_ip, strtok(buff, delim1)); // Stockage de l'addresse ip dans la structure root_server
        rs_tab->server_list[rs_tab->size].port = atoi(strtok(NULL, delim1));     // Stockage du numéro de port dans la structure root_server
        rs_tab->server_list[rs_tab->size].on = 1;
        rs_tab->size++; // Incrémentation de size pour connaitre le nombre de serveurs
    }

    fclose(fd);
    return rs_tab;
}
//#######################################################################

char *request(int sock, char *ip, int port, int id, char *name)
{
    int length;
    long perf;
    char *buffer = malloc(20000 * sizeof(char));
    struct sockaddr_in server;
    struct sockaddr_in from;
    socklen_t fromlen;
    struct timeval sendtime, recvtime, tv;
    tv.tv_sec = 0;
    tv.tv_usec = 10000;

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &(server.sin_addr.s_addr)); // Initialisation de l'adresse IP passée en paramètre
    server.sin_port = htons(port);               // Initialisation du numéro de port passé en paramètre

    fromlen = sizeof(struct sockaddr_in);

    snprintf(buffer, 2000, "%d|%ld|%s", id, time(NULL), name); // Création du message de la requête client

    gettimeofday(&sendtime, NULL);
    if (sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&server, sizeof(server)) < 0) // Envoi de la requête client au serveur
    {
        perror("erreur");
        return NULL;
    }

    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) // Gestion du temps d'attente du client en cas de panne de serveur
    {
        return NULL;
    }

    printf("sent : %s   to %d\n", buffer, port);

    // Réception de la réponse de la part du serveur
    if ((length = recvfrom(sock, buffer, 1024, 0, (struct sockaddr *)&from, &fromlen)) < 0)
    {
        strcat(buffer, "|-1");
        printf("timeout %d : %s\n\n", port, buffer);
        return NULL;
    }

    gettimeofday(&recvtime, NULL);

    perf = (1000000 * recvtime.tv_sec + recvtime.tv_usec) - (1000000 * sendtime.tv_sec + sendtime.tv_usec);

    buffer[length] = '\0';
    printf("received : %s\nperformance : %ld µs\n\n", buffer, perf);

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

    res->id = atoi(strtok(buffer, delim1)); // Récupération de l'id
    res->time = atol(strtok(NULL, delim1)); // Récupération de l'horodatage
    strtok(NULL, delim1);
    res->code = atoi(strtok(NULL, delim1)); // Réupération du code de retour

    char *matches[res->code];
    for (i = 0; i < res->code; i++)
    {
        matches[i] = malloc(300 * sizeof(char)); // Allocation mémoire
    }

    res->server_list = malloc(res->code * sizeof(server)); // Allocation mémoire

    for (i = 0; i < res->code; i++)
    {
        strcpy(matches[i], strtok(NULL, delim1)); // Récupération de la liste des serveurs correspondantss
    }

    for (j = 0; j < res->code; j++)
    {
        strcpy(res->server_list[j].url, strtok(matches[j], delim2)); // Récupération du nom
        strcpy(res->server_list[j].addr_ip, strtok(NULL, delim2));   // Récupération de l'adresse ip
        res->server_list[j].port = atoi(strtok(NULL, delim2));       // Récupération du numéro de port
        res->server_list[j].on = 1;
        free(matches[j]);
    }

    free(buffer);
    return res;
}
//#######################################################################

int main(int argc, char **argv)
{
    int sock, id = 1;
    char buffer[255];
    char *received;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) // Initialisation du socket
        error("socket");

    // Initialisation des structures
    root_server *rs_tab;
    server_response *s1;
    server_response *s2;
    server_response *s3;

    rs_tab = readFileRoot(argv[1]);
    buffer[0] = '\0';

    if (argc == 2)
    {
        // Récupération du nom à résoudre de l'entrée standard
        printf("Entrez le nom à résoudre : \n");
        scanf("%s", buffer);

        for (int i = 0; i < rs_tab->size && rs_tab->server_list[i].on != 0; i++) // Parcours des serveurs racines
        {
            if ((received = request(sock, rs_tab->server_list[i].addr_ip, rs_tab->server_list[i].port, id, buffer)) != NULL)
            {
                s1 = parse_server(received);
                for (int j = 0; j < s1->code && s1->server_list[j].on != 0; j++) // Parcours des serveurs domaines
                {
                    if ((received = request(sock, s1->server_list[j].addr_ip, s1->server_list[j].port, id + 1, buffer)) != NULL)
                    {
                        s2 = parse_server(received);
                        for (int k = 0; k < s2->code && s2->server_list[k].on != 0; k++) // Parcours des serveurs sous domaines
                        {
                            if ((received = request(sock, s2->server_list[k].addr_ip, s2->server_list[k].port, id + 2, buffer)) != NULL)
                            {
                                s3 = parse_server(received);
                                if (s3->code > 0) // Si on trouve le nom correspondant on termine les 3 boucles
                                {
                                    k = s2->code;
                                    j = s1->code;
                                    i = rs_tab->size;
                                    free(s3->server_list);
                                    free(s3);
                                    free(s2->server_list);
                                    free(s2);
                                    free(s1->server_list);
                                    free(s1);
                                }
                            }
                            else
                            {
                                s2->server_list[k].on = 0;
                            }
                        }
                    }
                    else
                    {
                        s1->server_list[j].on = 0;
                    }
                }
            }
            else
            {
                rs_tab->server_list[i].on = 0;
            }
            id += 3;
        }
    }
    else if (argc == 3)
    {
        FILE *fd;
        fd = fopen(argv[2], "r");
        char *buffer = malloc(151 * sizeof(char));
        size_t len = 150;
        ssize_t read;

        while ((read = getline(&buffer, &len, fd)) >= 0)
        {
            buffer[strcspn(buffer, "\n")] = '\0'; // Remplace le retour à la ligne par un EOF
            for (int i = 0; i < rs_tab->size && rs_tab->server_list[i].on != 0; i++)
            {
                if ((received = request(sock, rs_tab->server_list[i].addr_ip, rs_tab->server_list[i].port, id, buffer)) != NULL)
                {
                    s1 = parse_server(received);
                    for (int j = 0; j < s1->code && s1->server_list[j].on != 0; j++)
                    {
                        if ((received = request(sock, s1->server_list[j].addr_ip, s1->server_list[j].port, id + 1, buffer)) != NULL)
                        {
                            s2 = parse_server(received);
                            for (int k = 0; k < s2->code && s2->server_list[k].on != 0; k++)
                            {
                                if ((received = request(sock, s2->server_list[k].addr_ip, s2->server_list[k].port, id + 2, buffer)) != NULL)
                                {
                                    s3 = parse_server(received);
                                    if (s3->code > 0)
                                    {
                                        i = rs_tab->size;
                                        k = s2->code;
                                        j = s1->code;
                                        free(s3->server_list);
                                        free(s3);
                                        free(s2->server_list);
                                        free(s2);
                                        free(s1->server_list);
                                        free(s1);
                                    }
                                }
                                else
                                {
                                    s2->server_list[k].on = 0;
                                }
                            }
                        }
                        else
                        {
                            s1->server_list[j].on = 0;
                        }
                    }
                }
                else
                {
                    rs_tab->server_list[i].on = 0;
                }
            }
            id += 3;
        }
        fclose(fd);
        free(buffer);
    }
    else
    {
        error("Usage:\n    ./cli <servers_file>\nor\n    ./cli <servers_file> <names_file>\n");
    }

    free(rs_tab);
    close(sock);
    return 1;
}