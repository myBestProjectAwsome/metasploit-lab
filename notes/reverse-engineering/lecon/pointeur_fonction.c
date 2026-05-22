// pointeur fonctions

#include <stdio.h>
#include <stdlib.h>

int carre(int x)
{
    return x * x;
}

int cube(int x)
{
    return x * x * x;
}

int main(void)
{
    int (*op)(int);

    op = carre;
    printf("%d \n", op(4));
    op = cube;
    printf("%d\n",op(3));
    return 0;
}
