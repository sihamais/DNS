#include "name_server.h"

struct name_server *readFileName(char *filename)
{
    int fd, i = 0, j = 0;
    ssize_t n;
    char c;
    char *buff;

    struct name_server *ns_tab;
    ns_tab = malloc(1000 * sizeof(name_server));

    fd = open(filename, O_RDONLY);

    while ((n = read(fd, &c, 1)) > 0)
    {
        if (c == '|')
        {
            buff[j] = '\0';
            if (isdigit(buff[0]))
            {
                strcpy(ns_tab[i].addr_ip, buff);
            }
            else
            {
                strcpy(ns_tab[i].domain, buff);
            }
            j = 0;
        }
        else if (c == '\n')
        {
            buff[j] = '\0';
            ns_tab[i].port = atoi(buff);
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
    ns_tab[i].port = atoi(buff);
    i++;

    close(fd);
    return ns_tab;
}

int main(int argc, char **argv)
{
    struct name_server *ns_tab;
    ns_tab = readFileName(argv[1]);

    printf("Liste de serveurs noms:\n");
    for (int i = 0; i < 1000 && ns_tab[i].port != 0; i++)
    {
        printf("\n serveur %d:\n    domaine : %s\n    adresse ip : %s\n    port : %d\n\n", i+1, ns_tab[i].domain, ns_tab[i].addr_ip, ns_tab[i].port);
    }

    free(ns_tab);
    return 1;
}