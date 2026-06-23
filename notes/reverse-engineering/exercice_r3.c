#include <stdio.h>
#include <stdlib.h>

struct Cible
{
    char ip[16];
    int port;
    int ouvert;
};

void scanner(struct Cible *c)
{
    if (c->port == 21 || c->port == 22 || c->port == 443 || c->port == 80)
    {
        c->ouvert = 1;
    }
    else
    {
        c->ouvert = 0;
    }
}

int main(void)
{
    struct Cible *c1;
    struct Cible *c2;
    struct Cible *c3;
    struct Cible *c4;
    c1->port = 21;
    c2->port = 22;
    c3->port = 8080;
    c4->port = 14;
    struct Cible c[] = { c1, c2, c3, c4 };
    struct Cible *ptr = c;

    for (int i = 0; i < 4; i++)
    {
        scanner(ptr+i);
        printf("%d\n", ptr->port);
    }
    return 0;
}
