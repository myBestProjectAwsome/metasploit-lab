#include <stdio.h>
#include <stdlib.h>

int main()
{
    int tab[] = { 11, 22, 33, 44, 55 };
    int *debut = tab;
    int *fin = tab + 4;
    int tmp;

    while (debut < fin)
    {
        tmp = *debut;
        *debut = *fin;
        *fin = tmp;
        debut++;
        fin--;
    }

    int *p = tab;
    int i;
    for (i = 0; i < 5; i++)
    {
        printf("%d ", *(p + i));
    }
    printf("\n");

    return 0;
}
