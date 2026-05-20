# RE Leçon 1 — C Basique

## Pourquoi le C pour le reverse engineering ?

La majorité des binaires qu'on analyse (malwares, logiciels, CTF) sont
compilés depuis du C ou du C++. Comprendre le C permet de reconstruire
mentalement le code source depuis l'assembleur.

---

## 1. Variables et types

```c
#include <stdio.h>

int main() {
    int age = 25;           // entier 4 octets
    char lettre = 'A';      // caractère 1 octet
    float prix = 9.99;      // flottant 4 octets
    char nom[] = "Alice";   // chaîne de caractères

    printf("age: %d\n", age);
    printf("lettre: %c\n", lettre);
    printf("prix: %.2f\n", prix);
    printf("nom: %s\n", nom);

    return 0;
}
```

Compile et exécute :
```bash
gcc -o variables variables.c
./variables
```

**Ce que le RE retient** : chaque type a une taille fixe en mémoire.
En assembleur, `DWORD` = 4 octets (int), `BYTE` = 1 octet (char).

---

## 2. Fonctions

```c
#include <stdio.h>

int additionner(int a, int b) {
    int resultat = a + b;
    return resultat;
}

int main() {
    int x = additionner(3, 5);
    printf("résultat: %d\n", x);
    return 0;
}
```

**Ce que le RE retient** : chaque appel de fonction crée un **stack frame**.
Les arguments `a` et `b` sont passés via les registres `edi` et `esi`
(convention x64). La valeur de retour est dans `eax`.

---

## 3. Conditions

```c
#include <stdio.h>
#include <string.h>

int main() {
    char input[50];
    printf("Mot de passe : ");
    scanf("%s", input);

    if (strcmp(input, "secret123") == 0) {
        printf("Accès autorisé\n");
    } else {
        printf("Accès refusé\n");
    }

    return 0;
}
```

**Ce que le RE retient** : un `if` devient un `cmp` + `je/jne` en assembleur.
La chaîne `"secret123"` sera visible dans les strings du binaire.

---

## 4. Boucles

```c
#include <stdio.h>

int main() {
    int i;
    for (i = 0; i < 5; i++) {
        printf("i = %d\n", i);
    }
    return 0;
}
```

**Ce que le RE retient** : une boucle `for` devient un bloc avec
`cmp` + `jl/jge` + `jmp` en assembleur. Pattern très reconnaissable.

---

## 5. Pointeurs — le concept clé

Un pointeur stocke une **adresse mémoire** plutôt qu'une valeur directe.

```c
#include <stdio.h>

int main() {
    int x = 42;
    int *p = &x;        // p pointe vers x

    printf("valeur de x : %d\n", x);
    printf("adresse de x : %p\n", &x);
    printf("valeur via pointeur : %d\n", *p);

    *p = 100;           // modifier x via le pointeur
    printf("x après modification : %d\n", x);

    return 0;
}
```

**Ce que le RE retient** : en assembleur, tout est des adresses.
Comprendre les pointeurs = comprendre comment le code accède à la mémoire.

---

## 6. Chaînes de caractères (strings)

```c
#include <stdio.h>
#include <string.h>

int main() {
    char s1[] = "Bonjour";
    char s2[50];

    printf("longueur : %lu\n", strlen(s1));

    strcpy(s2, s1);         // copie s1 dans s2
    printf("copie : %s\n", s2);

    strcat(s2, " monde");   // concatène
    printf("concat : %s\n", s2);

    return 0;
}
```

**Ce que le RE retient** : `strcpy` sans vérification de taille = faille
buffer overflow. On retrouvera souvent ces fonctions dans les binaires.

---

## 7. Structs

```c
#include <stdio.h>

struct Utilisateur {
    char nom[32];
    int age;
    int admin;          // 0 = non, 1 = oui
};

int main() {
    struct Utilisateur u;
    u.age = 30;
    u.admin = 0;

    printf("admin : %d\n", u.admin);
    return 0;
}
```

**Ce que le RE retient** : les structs sont des blocs mémoire contigus.
En RE, identifier des structs permet de comprendre la logique d'un programme.

---

## Exercices pratiques

Crée ces fichiers dans `/tmp/c-lessons/` et compile chacun.

### Exercice 1 — Hello World

```c
// hello.c
#include <stdio.h>
int main() {
    printf("Hello, RE!\n");
    return 0;
}
```
```bash
gcc -o hello hello.c
./hello
```

### Exercice 2 — Vérification de mot de passe

```c
// password.c
#include <stdio.h>
#include <string.h>

int main() {
    char input[50];
    printf("Password: ");
    scanf("%49s", input);
    if (strcmp(input, "re4ever") == 0) {
        printf("WIN!\n");
    } else {
        printf("FAIL\n");
    }
    return 0;
}
```
```bash
gcc -o password password.c
./password
```

Puis cherche le mot de passe dans le binaire sans regarder le code :
```bash
strings password | grep -v "^lib\|^GCC\|^GNU\|GLIBC"
```

> Tu verras `re4ever` dans la sortie — c'est la base du reverse engineering.

### Exercice 3 — Pointeurs

```c
// pointeurs.c
#include <stdio.h>

void modifier(int *val) {
    *val = 999;
}

int main() {
    int x = 1;
    printf("avant : %d\n", x);
    modifier(&x);
    printf("après : %d\n", x);
    return 0;
}
```
```bash
gcc -o pointeurs pointeurs.c
./pointeurs
```

---

## Commandes de compilation utiles

```bash
gcc -o output source.c           # compilation basique
gcc -g -o output source.c        # avec symboles de debug (pour gdb)
gcc -O0 -o output source.c       # sans optimisation (plus lisible en RE)
gcc -m32 -o output source.c      # compiler en 32 bits
file output                      # identifier le binaire
strings output                   # extraire les chaînes
```

---

## Leçons apprises

- Chaque type C a une taille fixe → visible en assembleur
- Un `if` = `cmp` + saut conditionnel en asm
- Une boucle = bloc avec saut inconditionnel en asm
- Les strings hardcodées sont toujours visibles avec `strings`
- Les pointeurs = adresses mémoire = ce que tu manipules en RE

## Prochaine leçon

**Leçon 2 — Compilation et GDB** : compiler avec symboles de debug,
inspecter un programme avec gdb, voir la mémoire en direct.
