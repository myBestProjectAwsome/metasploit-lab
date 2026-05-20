# RE Leçon 2 — Exercices GDB

## Setup

```bash
cd /tmp/c-lessons
```

---

## Exercice 1 — Premier pas dans GDB

Compile le programme suivant avec symboles :

```c
// debug1.c
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
```

```bash
gcc -g -O0 -o debug1 debug1.c
gdb ./debug1
```

Dans GDB, fais dans l'ordre :

```bash
(gdb) break main
(gdb) run
(gdb) next          # avance ligne par ligne (3 fois)
(gdb) print x
(gdb) print y
(gdb) next
(gdb) print z
(gdb) continue
(gdb) quit
```

**Question** : quelle est la valeur de `z` juste après l'appel à `additionner` ?

---

## Exercice 2 — Inspecter les registres

Avec le même `debug1`, entre dans la fonction `additionner` :

```bash
gdb ./debug1
(gdb) break additionner
(gdb) run
(gdb) info registers
(gdb) print $rdi        # 1er argument
(gdb) print $rsi        # 2ème argument
(gdb) next
(gdb) print resultat
(gdb) print $rax        # valeur de retour après return
```

**Question** : dans quels registres sont passés `a` et `b` ?

---

## Exercice 3 — Modifier une variable à la volée

```c
// debug2.c
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
```

```bash
gcc -g -O0 -o debug2 debug2.c
gdb ./debug2
```

Objectif : **faire afficher "Accès admin accordé" sans modifier le code**.

```bash
(gdb) break main
(gdb) run
(gdb) next
(gdb) next
# on est sur le if
(gdb) set variable admin = 1
(gdb) continue
```

> C'est une technique réelle utilisée pour bypasser des vérifications
> de licence ou d'authentification.

---

## Exercice 4 — Intercepter strcmp (technique RE classique)

Reprends `crackme1` de la leçon 1. Recompile avec debug :

```bash
gcc -g -O0 -o crackme1_debug crackme1.c
gdb ./crackme1_debug
```

**Objectif** : trouver le mot de passe en interceptant `strcmp` dans GDB,
sans regarder le code source.

```bash
(gdb) break strcmp
(gdb) run
# entre n'importe quoi comme password quand demandé
(gdb) x/s $rdi      # notre input
(gdb) x/s $rsi      # le mot de passe attendu
```

Note le mot de passe trouvé, quitte GDB, et teste-le :

```bash
./crackme1_debug
```

---

## Exercice 5 — Désassembler et lire l'asm

```c
// debug3.c
#include <stdio.h>

int max(int a, int b) {
    if (a > b)
        return a;
    return b;
}

int main() {
    printf("%d\n", max(3, 7));
    return 0;
}
```

```bash
gcc -g -O0 -o debug3 debug3.c
gdb ./debug3
(gdb) disassemble max
```

Observe l'assembleur généré. Identifie :
- L'instruction `cmp` (comparaison)
- Le saut conditionnel (`jle`, `jg`, etc.)
- L'instruction `ret`

---

## Exercice 6 — Crackme sans symboles (RE réel)

Compile **sans** `-g` pour simuler un vrai binaire :

```c
// crackme2.c
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
```

```bash
gcc -O0 -o crackme2 crackme2.c   # sans -g intentionnellement
gdb ./crackme2
```

Cette fois, pas de symboles. Utilise :

```bash
(gdb) info functions       # lister les fonctions connues
(gdb) break verifier
(gdb) run
# entre n'importe quoi
(gdb) x/s $rsi             # trouver le secret
```

---

## Checkpoint

- [ ] Ex 1 : navigué dans GDB avec next/print
- [ ] Ex 2 : vu les registres rdi/rsi lors d'un appel de fonction
- [ ] Ex 3 : modifié une variable à la volée pour bypasser un if
- [ ] Ex 4 : trouvé un mot de passe en interceptant strcmp
- [ ] Ex 5 : lu de l'assembleur avec disassemble
- [ ] Ex 6 : cracké un binaire sans symboles de debug
