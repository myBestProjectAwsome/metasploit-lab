#include <stdio.h>

int main() {
    int admin = 0;

    printf("admin : %d\n", admin);

    if (admin == 1) {
        printf("Accès admin accordé\n");
    } else {
        printf("Accès refusé\n");
    }

    return 0;
}
