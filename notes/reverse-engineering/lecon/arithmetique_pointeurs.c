// arithmetique et pointeurs

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int i = 0;
    int tab[] = { 5, 10, 15, 20, 35 };

    int *p = NULL;
    
    p = tab;
    while (i < 5)
    {
        printf("adresse : %p et valeur : %d\n", p+ i, *(p+i));
        i++;
       
    }
    
    p = NULL;
    return 0;
}
