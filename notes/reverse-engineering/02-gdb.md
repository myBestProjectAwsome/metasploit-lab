# RE Leçon 2 — GDB : Déboguer et inspecter un binaire

## Qu'est-ce que GDB ?

GDB (GNU Debugger) permet d'exécuter un programme **pas à pas** et
d'inspecter la mémoire, les registres et la pile en temps réel.

En reverse engineering, GDB est utilisé pour l'**analyse dynamique** :
on observe le programme pendant qu'il tourne pour comprendre ce qu'il fait.

```
Analyse statique (Ghidra)     Analyse dynamique (GDB)
         |                              |
   lire le code                 exécuter pas à pas
   sans lancer                  voir les vraies valeurs
   le programme                 en mémoire au runtime
```

---

## Setup — compiler avec symboles de debug

Sans symboles, GDB affiche de l'assembleur brut.
Avec `-g`, GDB peut afficher le code C ligne par ligne.

```bash
# Sans symboles (ce qu'on analyse en RE réel)
gcc -o binary source.c

# Avec symboles (pour apprendre)
gcc -g -O0 -o binary source.c
```

> `-O0` désactive les optimisations du compilateur — le code généré
> est plus proche de ce qu'on a écrit, plus facile à lire.

---

## 1. Lancer GDB

```bash
gdb ./binary
```

Pour passer des arguments au programme :
```bash
gdb --args ./binary argument1 argument2
```

---

## 2. Commandes fondamentales

| Commande | Raccourci | Rôle |
|---|---|---|
| `run` | `r` | Lancer le programme |
| `quit` | `q` | Quitter GDB |
| `break main` | `b main` | Poser un breakpoint sur `main` |
| `break 12` | `b 12` | Breakpoint à la ligne 12 |
| `next` | `n` | Exécuter la ligne suivante (sans entrer dans les fonctions) |
| `step` | `s` | Exécuter la ligne suivante (entre dans les fonctions) |
| `continue` | `c` | Continuer jusqu'au prochain breakpoint |
| `print x` | `p x` | Afficher la valeur de la variable `x` |
| `info registers` | `i r` | Afficher tous les registres |
| `backtrace` | `bt` | Afficher la pile d'appels |
| `list` | `l` | Afficher le code source autour de la position courante |
| `disassemble` | `disas` | Désassembler la fonction courante |

---

## 3. Breakpoints

Un breakpoint = point d'arrêt. Le programme s'arrête là et te rend la main.

```bash
(gdb) break main          # arrêt au début de main
(gdb) break 15            # arrêt à la ligne 15
(gdb) break check_password # arrêt au début de la fonction
(gdb) info breakpoints    # lister tous les breakpoints
(gdb) delete 1            # supprimer le breakpoint n°1
```

---

## 4. Inspecter les variables

```bash
(gdb) print x             # valeur de x
(gdb) print &x            # adresse de x
(gdb) print *p            # valeur pointée par p
(gdb) print/x x           # valeur de x en hexadécimal
(gdb) print/s buffer      # afficher comme string
```

---

## 5. Inspecter la mémoire

```bash
(gdb) x/10x $rsp          # 10 valeurs hex à partir du stack pointer
(gdb) x/s 0x402000        # lire une string à l'adresse 0x402000
(gdb) x/i $rip            # instruction courante
(gdb) x/20i main          # 20 instructions depuis main
```

Format : `x/<nombre><format> <adresse>`

| Format | Signification |
|---|---|
| `x` | hexadécimal |
| `d` | décimal |
| `s` | string |
| `i` | instruction assembleur |
| `b` | byte (1 octet) |
| `w` | word (4 octets) |
| `g` | giant (8 octets) |

---

## 6. Inspecter les registres

```bash
(gdb) info registers          # tous les registres
(gdb) print $rax              # valeur de rax
(gdb) print $rsp              # stack pointer
(gdb) print $rip              # instruction pointer (position courante)
```

Registres x64 importants :

| Registre | Rôle |
|---|---|
| `rip` | Adresse de l'instruction courante |
| `rsp` | Sommet de la pile (stack pointer) |
| `rbp` | Base de la frame courante |
| `rax` | Valeur de retour des fonctions |
| `rdi` | 1er argument d'une fonction |
| `rsi` | 2ème argument |
| `rdx` | 3ème argument |

---

## 7. Modifier des valeurs à la volée

```bash
(gdb) set variable x = 999       # changer la valeur de x
(gdb) set $rax = 1               # changer un registre
```

Utile en RE pour tester des chemins d'exécution sans modifier le code.

---

## 8. Désassembler

```bash
(gdb) disassemble main           # désassembler main
(gdb) disassemble check_password # désassembler une fonction
```

Pour afficher les adresses et le code source en même temps :
```bash
(gdb) layout asm     # vue assembleur
(gdb) layout src     # vue code source
(gdb) layout regs    # vue registres
```

---

## Exemple complet — analyser crackme1

On reprend le `crackme1` de l'exercice 8 (leçon 1).

Recompile avec symboles de debug :
```bash
gcc -g -O0 -o crackme1_debug crackme1.c
gdb ./crackme1_debug
```

Dans GDB :

```bash
# Poser un breakpoint sur main
(gdb) break main
(gdb) run

# Le programme s'arrête au début de main
# Avancer ligne par ligne
(gdb) next
(gdb) next

# Quand on arrive sur strcmp, voir ce qui est comparé
(gdb) break strcmp
(gdb) continue

# Inspecter les arguments de strcmp
(gdb) print $rdi    # 1er argument : notre input
(gdb) print $rsi    # 2ème argument : le mot de passe attendu
(gdb) x/s $rsi      # afficher comme string → on voit "m4tr1x"
```

> Sans regarder le code source, on vient de trouver le mot de passe
> en interceptant l'appel à `strcmp` dans GDB.

---

## Leçons apprises

- GDB = microscope pour observer un programme en cours d'exécution
- `break strcmp` + `x/s $rsi` = technique classique pour trouver un mot de passe
- `-g -O0` lors de la compilation facilite l'analyse dans GDB
- `layout asm` + `layout regs` = vue complète pour l'analyse

## Prochaine leçon

**Leçon 3 — Assembleur x86/x64** : lire l'assembleur généré par GCC,
comprendre les patterns `if`, `boucle`, `appel de fonction` en asm.
