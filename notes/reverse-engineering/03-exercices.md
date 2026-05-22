# RE Leçon 3 — Exercices Assembleur

## Setup

```bash
cd /tmp/c-lessons
```

---

## Exercice 1 — Lire l'assembleur d'un if/else

Compile ce programme :

```c
// asm1.c
#include <stdio.h>
int main() {
    int x = 7;
    if (x > 5) {
        printf("grand\n");
    } else {
        printf("petit\n");
    }
    return 0;
}
```

```bash
gcc -g -O0 -o asm1 asm1.c
objdump -d -M intel asm1 | grep -A 40 "<main>:"
```

**Questions** :
1. Trouve l'instruction `cmp` — avec quelle valeur compare-t-elle ?
2. Quel saut conditionnel suit le `cmp` ?
3. Si tu changes `x = 3`, quel chemin prend le programme ?

Vérifie avec GDB :
```bash
gdb ./asm1
(gdb) break main
(gdb) run
(gdb) layout asm
(gdb) ni          # avance instruction par instruction
```

---

## Exercice 2 — Lire l'assembleur d'une boucle

```c
// asm2.c
#include <stdio.h>
int main() {
    int i;
    int somme = 0;
    for (i = 1; i <= 5; i++) {
        somme += i;
    }
    printf("somme = %d\n", somme);
    return 0;
}
```

```bash
gcc -g -O0 -o asm2 asm2.c
objdump -d -M intel asm2 | grep -A 60 "<main>:"
```

**Questions** :
1. Trouve l'initialisation `i = 1`
2. Trouve le `cmp` qui vérifie `i <= 5`
3. Trouve le `jmp` qui revient au début de la boucle
4. Trouve le `add` qui fait `somme += i`

---

## Exercice 3 — Tracer une fonction dans GDB

```c
// asm3.c
#include <stdio.h>

int calculer(int a, int b, int c) {
    return a * b + c;
}

int main() {
    int res = calculer(3, 4, 5);
    printf("res = %d\n", res);
    return 0;
}
```

```bash
gcc -g -O0 -o asm3 asm3.c
gdb ./asm3
```

Dans GDB :
```bash
(gdb) break calculer
(gdb) run
(gdb) info registers
(gdb) print $rdi      # 1er argument
(gdb) print $rsi      # 2ème argument
(gdb) print $rdx      # 3ème argument
(gdb) finish          # exécuter jusqu'au return
(gdb) print $rax      # valeur de retour
```

**Questions** :
1. Quelle est la valeur de `$rdi`, `$rsi`, `$rdx` à l'entrée de `calculer` ?
2. Quelle est la valeur de `$rax` après le `return` ?

---

## Exercice 4 — Identifier un crackme en lisant l'asm

```c
// crackme4.c
#include <stdio.h>
#include <string.h>

int main() {
    char input[50];
    int code;

    printf("Username: ");
    scanf("%49s", input);

    printf("Code: ");
    scanf("%d", &code);

    if (strcmp(input, "admin") == 0 && code == 1337) {
        printf("Flag: RE_{asm_reader}\n");
    } else {
        printf("Incorrect\n");
    }
    return 0;
}
```

```bash
gcc -O0 -o crackme4 crackme4.c   # sans -g
objdump -d -M intel crackme4 | grep -A 80 "<main>:"
```

**Objectif** : sans GDB, uniquement en lisant l'assembleur, trouver :
1. La chaîne comparée avec `strcmp`
2. La valeur comparée avec `cmp` pour le code

Indices dans l'asm :
- Cherche `call strcmp` — l'adresse avant = l'adresse de la string
- Cherche `cmp` avec une valeur immédiate = le code secret

Vérifie :
```bash
./crackme4
```

---

## Exercice 5 — Lire un XOR en assembleur

```c
// asm5.c
#include <stdio.h>
int main() {
    int a = 0xFF;
    int b = 0x0F;
    int c = a ^ b;
    int d = a & b;
    int e = a | b;
    printf("XOR: %02X\nAND: %02X\nOR:  %02X\n", c, d, e);
    return 0;
}
```

```bash
gcc -g -O0 -o asm5 asm5.c
objdump -d -M intel asm5 | grep -A 50 "<main>:"
```

**Questions** :
1. Trouve l'instruction `xor` dans l'assembleur
2. Trouve l'instruction `and`
3. Trouve l'instruction `or`

---

## Exercice 6 — Crackme XOR asm complet

```c
// crackme5.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    unsigned char enc[] = {0x36, 0x27, 0x30, 0x2B, 0x26, 0x00};
    int i, len = 5;
    char *pwd = malloc(len + 1);
    for (i = 0; i < len; i++)
        pwd[i] = enc[i] ^ 0x42;
    pwd[len] = '\0';

    char input[50];
    printf("Password: ");
    scanf("%49s", input);

    if (strcmp(input, pwd) == 0)
        printf("Flag: RE_{full_asm}\n");
    else
        printf("Wrong\n");

    free(pwd);
    return 0;
}
```

```bash
gcc -O0 -o crackme5 crackme5.c
```

**Méthode 1 — Python** (rapide) :
```python
enc = [0x36, 0x27, 0x30, 0x2B, 0x26]
print(''.join(chr(b ^ 0x42) for b in enc))
```

**Méthode 2 — GDB** (pratique RE) :
```bash
gdb ./crackme5
(gdb) break strcmp
(gdb) run
(gdb) continue   # passer le strcmp du loader
(gdb) x/s $rsi
```

---

## Checkpoint

- [ ] Ex 1 : identifié `cmp` + saut conditionnel dans un if/else
- [ ] Ex 2 : identifié la structure complète d'une boucle en asm
- [ ] Ex 3 : vu les arguments dans rdi/rsi/rdx + valeur de retour dans rax
- [ ] Ex 4 : trouvé username et code en lisant l'asm
- [ ] Ex 5 : identifié xor/and/or en asm
- [ ] Ex 6 : cracké via Python ou GDB
