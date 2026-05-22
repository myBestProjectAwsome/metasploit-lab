# RE Leçon 1b — Exercices C Avancé

## Setup

```bash
cd /tmp/c-lessons
```

---

## Exercice 1 — Arithmétique de pointeurs

Écris un programme qui :
- Déclare `int tab[] = {5, 10, 15, 20, 25}`
- Utilise **uniquement un pointeur** (pas d'index `tab[i]`) pour afficher
  chaque élément avec son adresse mémoire

Résultat attendu :
```
adresse 0x... : valeur 5
adresse 0x... : valeur 10
adresse 0x... : valeur 15
adresse 0x... : valeur 20
adresse 0x... : valeur 25
```

<details>
<summary>Solution</summary>

```c
#include <stdio.h>

int main() {
    int tab[] = {5, 10, 15, 20, 25};
    int *p = tab;
    int i;
    for (i = 0; i < 5; i++) {
        printf("adresse %p : valeur %d\n", (p+i), *(p+i));
    }
    return 0;
}
```
</details>

---

## Exercice 2 — malloc / free

Écris un programme qui :
- Demande un nombre `n` à l'utilisateur
- Alloue un tableau de `n` entiers avec `malloc`
- Remplit le tableau avec les valeurs `1, 2, ..., n`
- Affiche le tableau
- Libère la mémoire avec `free`

```bash
gcc -o ex2 ex2.c
./ex2
# Entrez n: 5
# 1 2 3 4 5
```

<details>
<summary>Solution</summary>

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    int n, i;
    printf("Entrez n: ");
    scanf("%d", &n);

    int *tab = (int *)malloc(n * sizeof(int));
    if (tab == NULL) {
        printf("Erreur malloc\n");
        return 1;
    }

    for (i = 0; i < n; i++)
        tab[i] = i + 1;

    for (i = 0; i < n; i++)
        printf("%d ", tab[i]);
    printf("\n");

    free(tab);
    return 0;
}
```
</details>

---

## Exercice 3 — Pointeurs de fonctions

Écris un programme avec :
- Une fonction `carre(int x)` qui retourne x²
- Une fonction `cube(int x)` qui retourne x³
- Dans `main` : un pointeur de fonction `op` qui appelle d'abord
  `carre(4)` puis `cube(3)`

Résultat attendu :
```
carre(4) = 16
cube(3) = 27
```

<details>
<summary>Solution</summary>

```c
#include <stdio.h>

int carre(int x) { return x * x; }
int cube(int x)  { return x * x * x; }

int main() {
    int (*op)(int);

    op = carre;
    printf("carre(4) = %d\n", op(4));

    op = cube;
    printf("cube(3) = %d\n", op(3));

    return 0;
}
```
</details>

---

## Exercice 4 — XOR (technique malware)

Écris un programme qui :
- Déclare `char msg[] = "HELLO"`
- Chiffre chaque caractère avec XOR `0x55`
- Affiche le résultat chiffré en hexadécimal
- Déchiffre et affiche le message original

Résultat attendu :
```
Chiffré  : 1D 30 39 39 3A
Déchiffré: HELLO
```

<details>
<summary>Solution</summary>

```c
#include <stdio.h>
#include <string.h>

int main() {
    char msg[] = "HELLO";
    char cle = 0x55;
    int i, len = strlen(msg);

    for (i = 0; i < len; i++)
        msg[i] ^= cle;

    printf("Chiffré  : ");
    for (i = 0; i < len; i++)
        printf("%02X ", (unsigned char)msg[i]);
    printf("\n");

    for (i = 0; i < len; i++)
        msg[i] ^= cle;

    printf("Déchiffré: %s\n", msg);
    return 0;
}
```
</details>

---

## Exercice 5 — Union (lire la mémoire byte par byte)

Écris un programme avec une union qui contient :
- Un `unsigned int` nommé `valeur`
- Un tableau de 4 `unsigned char` nommé `bytes`

Dans `main` :
- Assigne `0xDEADBEEF` à `valeur`
- Affiche chaque byte séparément en hexadécimal

Résultat attendu :
```
byte[0] = EF
byte[1] = BE
byte[2] = AD
byte[3] = DE
```

> Observe l'ordre des bytes — c'est le **little-endian** (les systèmes
> x86 stockent le byte de poids faible en premier).

<details>
<summary>Solution</summary>

```c
#include <stdio.h>

union Memoire {
    unsigned int valeur;
    unsigned char bytes[4];
};

int main() {
    union Memoire m;
    m.valeur = 0xDEADBEEF;

    int i;
    for (i = 0; i < 4; i++)
        printf("byte[%d] = %02X\n", i, m.bytes[i]);

    return 0;
}
```
</details>

---

## Exercice 6 — Crackme avancé (tout en un)

Compile ce binaire **sans regarder le code** après :

```c
// crackme3.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *decoder(char *encoded, int len) {
    char *result = (char *)malloc(len + 1);
    int i;
    for (i = 0; i < len; i++)
        result[i] = encoded[i] ^ 0x2A;
    result[len] = '\0';
    return result;
}

int main() {
    char encoded[] = {0x4B, 0x46, 0x5C, 0x5C, 0x41, 0x00};
    char *secret = decoder(encoded, 5);

    char input[50];
    printf("Password: ");
    scanf("%49s", input);

    if (strcmp(input, secret) == 0)
        printf("Flag: RE_{xor_decoder}\n");
    else
        printf("Wrong\n");

    free(secret);
    return 0;
}
```

```bash
gcc -O0 -o crackme3 crackme3.c
```

**Objectif** : trouver le mot de passe. Tu as deux méthodes :

**Méthode 1 — GDB** (ce qu'on a appris) :
```bash
gdb ./crackme3
(gdb) break strcmp
(gdb) run
# entre n'importe quoi
(gdb) continue   # passer le strcmp du loader
(gdb) x/s $rsi
```

**Méthode 2 — Analyse du code** (si tu veux comprendre) :
- Les bytes `{0x4B, 0x46, 0x5C, 0x5C, 0x41}` sont XOR avec `0x2A`
- Calcule chaque caractère à la main ou avec Python :

```python
encoded = [0x4B, 0x46, 0x5C, 0x5C, 0x41]
print(''.join(chr(b ^ 0x2A) for b in encoded))
```

---

## Checkpoint

- [ ] Ex 1 : arithmétique de pointeurs — tableau sans index
- [ ] Ex 2 : malloc/free — tableau dynamique
- [ ] Ex 3 : pointeurs de fonctions — carre/cube
- [ ] Ex 4 : XOR chiffrement/déchiffrement
- [ ] Ex 5 : union — little-endian visible
- [ ] Ex 6 : crackme XOR — trouvé via GDB ou calcul manuel
