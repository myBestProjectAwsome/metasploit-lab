#include <stdio.h>

int main(void)
{
    int res = 1;

    for (int i = 1; i <= 10; i++)
    {
        res = 3 * i;
        printf(" 3 * %d = %d\n", i, res);
    }
    return 0;
}
