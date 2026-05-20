// struct

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Joueur Joueur;

struct Joueur
{
    char nom[50];
    int score;
    int vip;
};

int main(void)
{
    Joueur j;
    strcpy(j.nom,"Bob");
    j.score = 500;
    j.vip = 1;
    return 0;
}
