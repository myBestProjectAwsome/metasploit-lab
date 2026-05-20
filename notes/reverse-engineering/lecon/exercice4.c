// crackme

// crackme1.c
#include <stdio.h>
#include <string.h>

int main()
{
    char input[50];
    printf("Password: ");
    scanf("%49s", input);
    if (strcmp(input, "m4tr1x") == 0)
    {
        printf("Flag: RE_{bien_joue}\n");
    }
    else
    {
        printf("Wrong password\n");
    }
    return 0;
}
