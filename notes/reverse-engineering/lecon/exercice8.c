// les pointeurs
#include <stdio.h>
int main(void)
{
    int x = 10;

    int *p = &x;

    printf("Adresse du pointeur : %p\n",p);

    *p = 99;
    printf("x = %d\n",x);

    return 0;
}
