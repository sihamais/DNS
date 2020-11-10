#include "name_server.h"

struct name_server * readFileName(char *filename)
{
    int fd, i = 0, j = 0;
    ssize_t n;
    char c;
    char *buff;

    struct name_server * ns_tab;
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
            }
            else
            {
                strcpy(ns_tab[i].addr_ip, buff);
            }
            j = 0;
        }
        else if (c == '\n' || n == 0)
        {
            char temp[6];
            buff[j] = '\0';
            strcpy(temp, buff);
            ns_tab[i].port = atoi(temp);
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
    i++;

    close(fd);
    return ns_tab;
}

void freeMemory(struct name_server * ns_tab)
{
    free(ns_tab);
}

int main(int argc, char ** argv){
    struct name_server * ns_tab;
    ns_tab = readFileName("name_server.txt");
    printf("Liste de serveurs noms:\n");
    printf("{\n serveur 1:\n");
    printf("    domaine : %s\n    adresse ip : %s\n    port : %d\n},\n",ns_tab[0].domain, ns_tab[0].addr_ip, ns_tab[0].port);
    printf("{\n serveur 2:\n");
    printf("    domaine : %s\n    adresse ip : %s\n    port : %d\n}\n",ns_tab[1].domain, ns_tab[1].addr_ip, ns_tab[1].port);
    return 1;
}