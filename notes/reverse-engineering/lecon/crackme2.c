#include <stdio.h>
#include <string.h>

int verifier(char *input) {
    char secret[] = "gh0st";
    return strcmp(input, secret) == 0;
}

int main() {
    char input[50];
    printf("Code secret: ");
    scanf("%49s", input);

    if (verifier(input)) {
        printf("Validé ! Flag: RE_{gdb_master}\n");
    } else {
        printf("Incorrect\n");
    }
    return 0;
}
