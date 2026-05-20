#include <stdio.h>

int main(void)
{
    int N = 0;
    puts("Veuillez saisir un nombre\n");
    scanf("%d", &N);
    if (N == 1337)
    {
        puts("Acces Granted\n");
    }
    else
    {
        puts("Acces denied\n");
    }
    return 0;
}
