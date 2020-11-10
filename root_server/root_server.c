#include "root_server.h"

struct root_server * readFileRoot(char *filename)
{
    int fd, i = 0, j = 0;
    ssize_t n;
    char c;
    char *buff;

    struct root_server *rs_tab;
    rs_tab = malloc(1000 * sizeof(root_server));

    fd = open(filename, O_RDONLY);

    while ((n = read(fd, &c, 1)) > 0)
    {
        if (c == '|')
        {
            buff[j] = '\0';
            strcpy(rs_tab[i].addr_ip, buff);
            printf("%s\n", rs_tab[i].addr_ip);
            j = 0;
        }
        else if (c == '\n' || n == 0)
        {
            char temp[6];
            buff[j] = '\0';
            strcpy(temp, buff);
            rs_tab[i].port = atoi(temp);
            printf("%d\n", rs_tab[i].port);
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
    rs_tab[i].port = atoi(temp);
    printf("%d\n", rs_tab[i].port);
    i++;

    close(fd);
    return rs_tab;
}

void freeMemory(struct root_server * rs_tab)
{
    free(rs_tab);
}