// exercice sur malloc et free

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int n = 0;
    puts("Veuillez saisir un nombre : \n");
    scanf("%d", &n);
    printf("L entier saisit est %d\n", n);

    int *tab = malloc(sizeof(int) * n);
    puts("on remplit le tableau maintenant\n");

    for (int i = 1; i <= n; i++)
    {
        tab[i] = i;
        printf("%d \n", tab[i]);
    }
    free(tab);
    tab = NULL;
    return 0;
}
