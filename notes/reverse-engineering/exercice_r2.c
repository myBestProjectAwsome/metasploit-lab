#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
int is_voyelles(char c)
{
    if (c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U' || c == 'Y')
    {
        return 1;
    }
    else if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u'
             || c == 'y')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int compter_voyelles(char *s)
{
    int i = 0;
    int cpt = 0;
    while (s[i] != '\0')
    {
        if (is_voyelles(s[i]))
        {
            cpt++;
        }
        i++;
    }
    return cpt;
}

int main(void)
{
    assert(compter_voyelles("AAAA") == 4);
    assert(compter_voyelles("Bilel Majdoub") == 5);
    assert(compter_voyelles("AAAA") != 3);
    return 0;
}
