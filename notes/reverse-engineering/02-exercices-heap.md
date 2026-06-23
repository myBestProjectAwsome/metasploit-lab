# RE — Exercices Heap (malloc / free / realloc / calloc)

Focus sur la mémoire dynamique. En RE, chaque `call malloc` est un indice
sur les structures de données du programme. Savoir lire le heap = savoir
comprendre un malware.

```bash
cd notes/reverse-engineering/lecon
```

Compile toujours avec : `gcc -g -O0 -o nom nom.c`

---

## Exercice H1 — Tableau dynamique qui grandit (malloc + realloc)

Écris un programme qui :
- Commence avec un tableau de 2 entiers (`malloc`)
- Demande des nombres à l'utilisateur en boucle
- Quand le tableau est plein, double sa taille avec `realloc`
- L'utilisateur tape `-1` pour arrêter
- Affiche tous les nombres saisis

Résultat attendu :
```
Nombre (-1 pour stop): 10
Nombre (-1 pour stop): 20
Nombre (-1 pour stop): 30
  [realloc: 2 -> 4]
Nombre (-1 pour stop): 40
Nombre (-1 pour stop): 50
  [realloc: 4 -> 8]
Nombre (-1 pour stop): -1
Résultat: 10 20 30 40 50
```

Indice RE : `realloc` peut déplacer le bloc en mémoire. En asm, le
nouveau pointeur retourné dans `rax` peut être différent de l'ancien.

<details>
<summary>Solution</summary>

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    int capacite = 2;
    int taille = 0;
    int *tab = (int *)malloc(capacite * sizeof(int));
    if (!tab) return 1;

    int val;
    while (1) {
        printf("Nombre (-1 pour stop): ");
        scanf("%d", &val);
        if (val == -1)
            break;

        if (taille >= capacite) {
            capacite *= 2;
            printf("  [realloc: %d -> %d]\n", capacite / 2, capacite);
            tab = (int *)realloc(tab, capacite * sizeof(int));
            if (!tab) return 1;
        }

        tab[taille] = val;
        taille++;
    }

    printf("Résultat: ");
    int i;
    for (i = 0; i < taille; i++)
        printf("%d ", tab[i]);
    printf("\n");

    free(tab);
    return 0;
}
```
</details>

---

## Exercice H2 — calloc vs malloc (calloc)

Écris un programme qui :
- Alloue un tableau de 10 entiers avec `malloc`
- Affiche les 10 valeurs (elles sont **non initialisées** → garbage)
- Alloue un tableau de 10 entiers avec `calloc`
- Affiche les 10 valeurs (elles sont **à zéro**)

Résultat attendu :
```
malloc  : 0 0 1234567 -42 ... (valeurs aléatoires)
calloc  : 0 0 0 0 0 0 0 0 0 0
```

Indice RE : `calloc(n, size)` = `malloc(n*size)` + `memset(0)`.
En asm tu verras un `call calloc` ou un `call malloc` suivi de
`rep stosb` (remplissage à zéro).

<details>
<summary>Solution</summary>

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    int *m = (int *)malloc(10 * sizeof(int));
    int *c = (int *)calloc(10, sizeof(int));
    int i;

    printf("malloc  : ");
    for (i = 0; i < 10; i++)
        printf("%d ", m[i]);
    printf("\n");

    printf("calloc  : ");
    for (i = 0; i < 10; i++)
        printf("%d ", c[i]);
    printf("\n");

    free(m);
    free(c);
    return 0;
}
```
</details>

---

## Exercice H3 — Tableau de strings dynamique (malloc + malloc)

Écris un programme qui :
- Demande combien de noms l'utilisateur veut entrer (`n`)
- Alloue un tableau de `n` pointeurs `char*` avec `malloc`
- Pour chaque nom : alloue un buffer de 64 bytes, lit le nom dedans
- Affiche tous les noms avec leur adresse heap
- Libère **chaque buffer** puis le tableau de pointeurs

Résultat attendu :
```
Combien de noms ? 3
Nom 1: Alice
Nom 2: Bob
Nom 3: Charlie
---
[0x5555...] Alice
[0x5555...] Bob
[0x5555...] Charlie
```

Indice RE : un `char **` = tableau de pointeurs vers des strings.
En asm : `mov rax, [rbx+i*8]` pour accéder au i-ème pointeur,
puis `mov al, [rax]` pour lire le premier char de la string.

<details>
<summary>Solution</summary>

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    int n, i;
    printf("Combien de noms ? ");
    scanf("%d", &n);

    char **noms = (char **)malloc(n * sizeof(char *));
    if (!noms) return 1;

    for (i = 0; i < n; i++) {
        noms[i] = (char *)malloc(64);
        if (!noms[i]) return 1;
        printf("Nom %d: ", i + 1);
        scanf("%63s", noms[i]);
    }

    printf("---\n");
    for (i = 0; i < n; i++)
        printf("[%p] %s\n", noms[i], noms[i]);

    for (i = 0; i < n; i++)
        free(noms[i]);
    free(noms);

    return 0;
}
```
</details>

---

## Exercice H4 — Struct sur le heap (malloc + struct + pointeurs)

Crée une struct `Process` :
```c
struct Process {
    int pid;
    char name[32];
    int malicious;  // 0 ou 1
};
```

Écris :
- `struct Process *create_process(int pid, char *name, int mal)` → alloue
  un `Process` sur le heap, remplit les champs, retourne le pointeur
- `void print_process(struct Process *p)` → affiche les infos
- `void kill_process(struct Process *p)` → affiche un message et `free`

Dans `main` :
- Crée 3 processus avec `create_process`
- Affiche-les
- Free-les avec `kill_process`

Résultat attendu :
```
[PID 1234] svchost.exe     [CLEAN]
[PID 6666] backdoor.exe    [MALICIOUS]
[PID 4321] explorer.exe    [CLEAN]
Killed PID 1234
Killed PID 6666
Killed PID 4321
```

Indice RE : en malware analysis, les structs allouées sur le heap
contiennent souvent les config C2 (IP, port, clés). Pattern :
`call malloc` → `mov [rax], valeur` → `mov [rax+4], valeur`.

<details>
<summary>Solution</summary>

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Process {
    int pid;
    char name[32];
    int malicious;
};

struct Process *create_process(int pid, char *name, int mal) {
    struct Process *p = (struct Process *)malloc(sizeof(struct Process));
    if (!p) return NULL;
    p->pid = pid;
    strncpy(p->name, name, 31);
    p->name[31] = '\0';
    p->malicious = mal;
    return p;
}

void print_process(struct Process *p) {
    printf("[PID %d] %-18s [%s]\n",
        p->pid, p->name,
        p->malicious ? "MALICIOUS" : "CLEAN");
}

void kill_process(struct Process *p) {
    printf("Killed PID %d\n", p->pid);
    free(p);
}

int main() {
    struct Process *procs[3];
    procs[0] = create_process(1234, "svchost.exe", 0);
    procs[1] = create_process(6666, "backdoor.exe", 1);
    procs[2] = create_process(4321, "explorer.exe", 0);

    int i;
    for (i = 0; i < 3; i++)
        print_process(procs[i]);
    for (i = 0; i < 3; i++)
        kill_process(procs[i]);

    return 0;
}
```
</details>

---

## Exercice H5 — Use-After-Free (bug classique)

Écris un programme qui **démontre** un use-after-free :

1. `malloc` un buffer de 32 bytes, copie `"CONFIDENTIEL"` dedans
2. Affiche le contenu
3. `free` le buffer
4. `malloc` un **nouveau** buffer de 32 bytes, copie `"AAAAAAAAAA"` dedans
5. Affiche l'**ancien** pointeur (celui qui a été free)

Questions à te poser :
- Est-ce que l'ancien pointeur affiche encore "CONFIDENTIEL" ?
- Est-ce qu'il affiche "AAAAAAAAAA" ?
- Pourquoi ?

```
Buffer 1 : CONFIDENTIEL (adresse: 0x5555...)
[free buffer 1]
Buffer 2 : AAAAAAAAAA   (adresse: 0x5555...)
Buffer 1 (après free) : AAAAAAAAAA   <-- USE AFTER FREE!
```

Indice RE : le UAF est une des vulnérabilités les plus exploitées.
Après `free`, l'allocateur peut réutiliser la même zone mémoire.
L'ancien pointeur pointe vers les nouvelles données → fuite d'info
ou contrôle du flux d'exécution.

<details>
<summary>Solution</summary>

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char *buf1 = (char *)malloc(32);
    strcpy(buf1, "CONFIDENTIEL");
    printf("Buffer 1 : %s (adresse: %p)\n", buf1, buf1);

    char *save = buf1;

    free(buf1);
    printf("[free buffer 1]\n");

    char *buf2 = (char *)malloc(32);
    strcpy(buf2, "AAAAAAAAAA");
    printf("Buffer 2 : %s (adresse: %p)\n", buf2, buf2);

    printf("Buffer 1 (après free) : %s  <-- USE AFTER FREE!\n", save);

    if (save == buf2)
        printf("MEME ADRESSE! L'allocateur a réutilisé le bloc.\n");

    free(buf2);
    return 0;
}
```
</details>

---

## Exercice H6 — Double Free (crash contrôlé)

Écris un programme qui :
1. `malloc` un buffer
2. `free` le buffer
3. `free` le buffer **une deuxième fois** (double free)

Compile et exécute :
```bash
gcc -g -O0 -o double_free double_free.c
./double_free
```

Observe le message d'erreur (`double free or corruption`).

Puis corrige le bug : après chaque `free`, mets le pointeur à `NULL`
et vérifie avant de free.

Indice RE : le double free corrompt les métadonnées internes du heap
(les free lists). Les exploits heap utilisent ça pour obtenir un
`write-what-where` — écrire une valeur arbitraire à une adresse
arbitraire.

<details>
<summary>Solution (version buggée)</summary>

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    char *buf = (char *)malloc(32);
    printf("alloc: %p\n", buf);

    free(buf);
    printf("premier free OK\n");

    free(buf);  // CRASH: double free
    printf("deuxième free (jamais atteint)\n");

    return 0;
}
```
</details>

<details>
<summary>Solution (version corrigée)</summary>

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    char *buf = (char *)malloc(32);
    printf("alloc: %p\n", buf);

    free(buf);
    buf = NULL;
    printf("premier free OK\n");

    if (buf != NULL)
        free(buf);
    printf("pas de double free\n");

    return 0;
}
```
</details>

---

## Exercice H7 — Memory leak detector maison (malloc + compteur)

Écris un mini-système de tracking mémoire :

- Variable globale `int alloc_count = 0`
- Fonction `void *tracked_malloc(size_t size)` → appelle `malloc`,
  incrémente `alloc_count`, affiche l'allocation, retourne le pointeur
- Fonction `void tracked_free(void *ptr)` → appelle `free`,
  décrémente `alloc_count`, affiche la libération
- Fonction `void check_leaks(void)` → affiche si `alloc_count == 0`
  (pas de leak) ou combien de blocs fuient

Dans `main`, fais exprès de :
- Allouer 5 blocs
- N'en libérer que 3
- Appeler `check_leaks` → doit détecter 2 leaks

Résultat attendu :
```
[+] malloc 32 bytes -> 0x5555...  (total: 1)
[+] malloc 64 bytes -> 0x5555...  (total: 2)
[+] malloc 16 bytes -> 0x5555...  (total: 3)
[+] malloc 128 bytes -> 0x5555... (total: 4)
[+] malloc 8 bytes -> 0x5555...   (total: 5)
[-] free 0x5555...  (total: 4)
[-] free 0x5555...  (total: 3)
[-] free 0x5555...  (total: 2)
[!] LEAK DETECTED: 2 blocs non libérés!
```

Indice RE : les outils comme Valgrind font exactement ça (en plus
sophistiqué). En CTF, les leaks heap peuvent être exploités pour
lire des adresses et contourner l'ASLR.

<details>
<summary>Solution</summary>

```c
#include <stdio.h>
#include <stdlib.h>

int alloc_count = 0;

void *tracked_malloc(size_t size) {
    void *ptr = malloc(size);
    alloc_count++;
    printf("[+] malloc %zu bytes -> %p  (total: %d)\n", size, ptr, alloc_count);
    return ptr;
}

void tracked_free(void *ptr) {
    free(ptr);
    alloc_count--;
    printf("[-] free %p  (total: %d)\n", ptr, alloc_count);
}

void check_leaks(void) {
    if (alloc_count == 0)
        printf("[OK] Pas de leak\n");
    else
        printf("[!] LEAK DETECTED: %d blocs non libérés!\n", alloc_count);
}

int main() {
    void *a = tracked_malloc(32);
    void *b = tracked_malloc(64);
    void *c = tracked_malloc(16);
    void *d = tracked_malloc(128);
    void *e = tracked_malloc(8);

    tracked_free(a);
    tracked_free(c);
    tracked_free(e);

    check_leaks();

    tracked_free(b);
    tracked_free(d);
    return 0;
}
```
</details>

---

## Exercice H8 — Heap buffer overflow (vulnérabilité)

Écris un programme avec deux buffers **consécutifs** sur le heap :

```c
char *username = malloc(16);
char *role = malloc(16);
strcpy(role, "user");
```

Puis lis un username avec `gets` (ou `strcpy` d'une string trop longue)
qui **dépasse** les 16 bytes et **écrase** le contenu de `role`.

Objectif : faire en sorte que `role` devienne `"admin"` sans jamais
écrire directement dans `role`.

Résultat attendu :
```
role avant : user
Entrez username: AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
role après : AAAA...  (écrasé!)
```

Note : le comportement exact dépend de l'allocateur. Compile avec
`gcc -g -O0 -z execstack -fno-stack-protector` pour maximiser les
chances de voir l'overflow.

Indice RE : le heap overflow est la technique derrière beaucoup
d'exploits réels. Les métadonnées malloc (chunk headers) entre
les blocs peuvent aussi être corrompues.

<details>
<summary>Solution</summary>

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char *username = (char *)malloc(16);
    char *role = (char *)malloc(16);
    strcpy(role, "user");

    printf("username @ %p\n", username);
    printf("role     @ %p\n", role);
    printf("distance : %ld bytes\n", (long)(role - username));
    printf("role avant : %s\n", role);

    printf("Entrez username: ");
    // VULNERABLE: pas de limite de taille
    // En vrai code: JAMAIS faire ça
    scanf("%s", username);

    printf("role après : %s\n", role);

    if (strcmp(role, "user") != 0)
        printf("[!] HEAP OVERFLOW: role a été écrasé!\n");

    free(username);
    free(role);
    return 0;
}
```

Pour tester : entre une chaîne de 40+ caractères.
</details>

---

## Récap patterns RE (heap)

| Pattern C | Ce que tu vois en assembleur |
|---|---|
| `malloc(n)` | `mov rdi, n` → `call malloc` → résultat dans `rax` |
| `free(p)` | `mov rdi, p` → `call free` |
| `calloc(n, s)` | `mov rdi, n` → `mov rsi, s` → `call calloc` |
| `realloc(p, n)` | `mov rdi, p` → `mov rsi, n` → `call realloc` |
| Use-After-Free | accès via un pointeur après un `call free` |
| Double Free | deux `call free` avec le même argument |
| Heap Overflow | écriture au-delà de la taille allouée |

---

## Checkpoint

- [ ] H1 : realloc — tableau qui grandit dynamiquement
- [ ] H2 : calloc vs malloc — observer l'initialisation
- [ ] H3 : tableau de strings (malloc de pointeurs + malloc de buffers)
- [ ] H4 : struct sur le heap (create/print/kill)
- [ ] H5 : use-after-free — observer la réutilisation mémoire
- [ ] H6 : double free — crash + correction
- [ ] H7 : leak detector maison
- [ ] H8 : heap buffer overflow — écraser un buffer adjacent
