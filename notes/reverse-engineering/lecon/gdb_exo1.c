// fonction additionner


#include <stdio.h>

int additionner(int a, int b) {
    int resultat = a + b;
    return resultat;
}

int main() {
    int x = 10;
    int y = 32;
    int z = additionner(x, y);
    printf("résultat : %d\n", z);
    return 0;
}
