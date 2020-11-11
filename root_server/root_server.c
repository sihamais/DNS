#include "root_server.h"

struct root_server * readFileRoot(char * filename)
{
    int fd, i = 0, j = 0;
    ssize_t n;
    char c;
    char *buff;

    struct root_server * rs_tab;
    rs_tab = malloc(1000 * sizeof(root_server));

    fd = open(filename, O_RDONLY);

    while ((n = read(fd, &c, 1)) > 0)
    {
        if (c == '|')
        {
            buff[j] = '\0';
            strcpy(rs_tab[i].addr_ip, buff);
            j = 0;
        }
        else if (c == '\n')
        {
            buff[j] = '\0';
            rs_tab[i].port = atoi(buff);
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
    rs_tab[i].port = atoi(buff);
    i++;

    close(fd);
    return rs_tab;
}

int main(int argc, char ** argv){
    struct root_server * rs_tab;
    rs_tab = readFileRoot(argv[1]);

    printf("Liste de serveurs racines:\n");
    for (int i = 0; i < 1000 && rs_tab[i].port != 0; i++){
        printf("\n serveur %d :\n    adresse ip : %s\n    port : %d\n\n",i, rs_tab[i].addr_ip, rs_tab[i].port);
    }

    free(rs_tab);
    return 1;
}