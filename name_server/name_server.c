#include "name_server.h"

struct name_server * readFileName(char *filename)
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
            if (!isdigit(buff[0]))
            {
                strcpy(ns_tab[i].domain, buff);
                printf("%s\n", ns_tab[i].domain);
            }
            else
            {
                strcpy(ns_tab[i].addr_ip, buff);
                printf("%s\n", ns_tab[i].addr_ip);
            }
            j = 0;
        }
        else if (c == '\n' || n == 0)
        {
            char temp[6];
            buff[j] = '\0';
            strcpy(temp, buff);
            ns_tab[i].port = atoi(temp);
            printf("%d\n", ns_tab[i].port);
            j = 0;
            i++;
        }
        else
        {
            buff[j] = c;
            j++;
        }
    }

    char temp[6];
    buff[j] = '\0';
    strcpy(temp, buff);
    ns_tab[i].port = atoi(temp);
    printf("%d\n", ns_tab[i].port);
    i++;

    close(fd);
    return ns_tab;
}

void freeMemory(struct name_server * ns_tab)
{
    free(ns_tab);
}