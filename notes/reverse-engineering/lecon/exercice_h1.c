#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int *tab = malloc(sizeof(int) * 2);
    int *ptr = NULL;

    for (int i = 0; i < 2; i++)
    {
        puts("Veuillez saisir un nombre !\n");
        scanf("%d", &tab[i]);
        printf("Vous venez de saisir : %d\n", tab[i]);
    }
    // arrivez ici, le tableau est plein

    ptr = realloc(tab, sizeof(int));
    // on affiche le tableau maintenant
    for (int i = 0; i < 3; i++)
    {
        printf("%d\n", tab[i]);
    }
    free(ptr);
    

    return 0;
}
