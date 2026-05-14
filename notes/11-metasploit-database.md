# Module #11 — Base de données Metasploit

## Informations

| Champ | Valeur |
|---|---|
| Date | 2026-05-13 |
| Cible | Metasploitable 2 (192.168.100.2) |
| Attaquant | Kali Linux (192.168.100.3) |
| Objectif | Utiliser la DB Metasploit pour structurer le pentest |

## Pourquoi utiliser la base de données ?

Sans DB, chaque scan est perdu dès que tu fermes msfconsole.
Avec la DB, tout est stocké : hosts, ports, services, credentials, loot.

```
Sans DB                          Avec DB
   |                                |
scan nmap → affiché → perdu     db_nmap → stocké → requêtable
credentials → notés à la main   creds → stockés automatiquement
fichiers récupérés → perdus      loot → indexés et retrouvables
```

---

## Architecture

Metasploit utilise **PostgreSQL** comme backend.

```
msfconsole
    |
    └── PostgreSQL (base "msf")
            |
            ├── hosts      (machines découvertes)
            ├── services   (ports/services par host)
            ├── vulns      (vulnérabilités détectées)
            ├── creds      (credentials capturés)
            └── loot       (fichiers/hashes récupérés)
```

---

## 0. Vérifier la connexion à la DB

```bash
db_status
```

Résultat attendu :
```
[*] Connected to msf. Connection type: postgresql.
```

Si non connecté :
```bash
# Depuis le terminal Kali (hors msfconsole)
service postgresql start
msfdb init
msfdb start
```

Puis relancer msfconsole.

---

## 1. Workspaces — organiser les engagements

Un workspace = un périmètre d'audit isolé. Tout ce qui est scanné/exploité
est rattaché au workspace actif.

```bash
workspace           # liste les workspaces
workspace -a lab    # créer et activer "lab"
workspace lab       # basculer sur "lab"
workspace -d lab    # supprimer "lab"
```

> Bonne pratique : créer un workspace par cible ou par engagement.

---

## 2. db_nmap — scanner et stocker les résultats

Remplace `nmap` en ligne de commande. Même syntaxe, résultats stockés en DB.

```bash
db_nmap -sV -sC -O 192.168.100.2
```

| Flag | Rôle |
|---|---|
| `-sV` | détecter les versions des services |
| `-sC` | scripts NSE par défaut |
| `-O` | détecter l'OS |
| `-p-` | scanner tous les ports |
| `-A` | tout activer (OS + versions + scripts) |

Résultat : les données sont automatiquement peuplées dans `hosts` et `services`.

---

## 3. hosts — visualiser les machines

```bash
hosts
```

```
Hosts
=====

address         mac  name           os_name  os_flavor  os_sp  purpose  info  comments
-------         ---  ----           -------  ---------  -----  -------  ----  --------
192.168.100.2        metasploitable  Linux    Ubuntu     2.6.X  server
```

### Filtres utiles

```bash
hosts -c address,os_name    # afficher seulement certaines colonnes
hosts -S linux              # filtrer par OS
hosts -R                    # charger les hosts dans RHOSTS automatiquement
```

---

## 4. services — visualiser les ports/services

```bash
services
```

```
Services
========

host            port  proto  name         state  info
----            ----  -----  ----         -----  ----
192.168.100.2   21    tcp    ftp          open   vsftpd 2.3.4
192.168.100.2   22    tcp    ssh          open   OpenSSH 4.7p1
192.168.100.2   23    tcp    telnet       open
192.168.100.2   25    tcp    smtp         open
192.168.100.2   80    tcp    http         open   Apache 2.2.8
192.168.100.2   139   tcp    netbios-ssn  open   Samba 3.x
192.168.100.2   445   tcp    microsoft-ds open   Samba 3.x
192.168.100.2   3306  tcp    mysql        open   MySQL 5.0.51a
192.168.100.2   5432  tcp    postgresql   open   PostgreSQL 8.3
192.168.100.2   6667  tcp    irc          open   UnrealIRCd
...
```

### Filtres utiles

```bash
services -p 80              # filtrer par port
services -s ftp             # filtrer par nom de service
services -S open            # seulement les ports ouverts
services -R                 # charger dans RHOSTS
```

---

## 5. vulns — vulnérabilités détectées

Peuplé automatiquement quand un module détecte ou exploite une vulnérabilité.

```bash
vulns
```

```
Vulnerabilities
===============

Timestamp  Host            Name                     References
---------  ----            ----                     ----------
...        192.168.100.2   vsftpd 2.3.4 Backdoor    CVE-2011-2523
...        192.168.100.2   Samba usermap_script      CVE-2007-2447
```

---

## 6. creds — credentials capturés

Peuplé automatiquement par les modules d'authentification et post-exploit.

```bash
creds
```

```
Credentials
===========

host            origin          service        public    private          realm  private_type
----            ------          -------        ------    -------          -----  ------------
192.168.100.2   192.168.100.2   5432/tcp (pg)  postgres  postgres                Password
192.168.100.2   192.168.100.2   3306/tcp (my)  root                              Blank password
```

### Ajouter des credentials manuellement

```bash
creds add host:192.168.100.2 port:80 user:admin password:password
```

---

## 7. loot — fichiers et données récupérés

Peuplé automatiquement par `hashdump`, `download`, et modules `post/gather`.

```bash
loot
```

```
Loot
====

host            service  type                        name         content    info
----            -------  ----                        ----         -------    ----
192.168.100.2            linux.passwd                passwd       text/plain Linux passwd file
192.168.100.2            linux.shadow                shadow       text/plain Linux shadow file
192.168.100.2            linux.hashes                hashes.txt   text/plain Linux password hashes
```

---

## 8. notes — annotations sur les hosts

```bash
notes
```

Ajouter une note manuellement :

```bash
notes -a -h 192.168.100.2 -t recon -n "vsftpd 2.3.4 backdoor confirmé sur port 21"
```

---

## Workflow complet d'un pentest structuré

```bash
# 1. Créer un workspace
workspace -a metasploitable-lab

# 2. Scanner la cible
db_nmap -sV -sC -O 192.168.100.2

# 3. Visualiser ce qui a été trouvé
hosts
services

# 4. Identifier les cibles
services -p 5432    # PostgreSQL → exploit #3
services -p 21      # FTP vsftpd → exploit #4
services -p 6667    # IRC → exploit futur

# 5. Exploiter et laisser la DB se peupler automatiquement
use exploit/linux/postgres/postgres_payload
# ... run ...

# 6. Post-exploitation → loot automatique
run post/linux/gather/hashdump

# 7. Consulter ce qui a été récupéré
creds
loot
vulns
```

---

## Exporter les données

```bash
# Exporter les hosts en CSV
hosts -o /tmp/hosts.csv

# Exporter les services en CSV
services -o /tmp/services.csv

# Exporter les credentials
creds -o /tmp/creds.csv
```

---

## Récapitulatif des commandes

| Commande | Rôle |
|---|---|
| `db_status` | Vérifier la connexion DB |
| `workspace -a <nom>` | Créer un workspace |
| `db_nmap <options> <cible>` | Scanner + stocker |
| `hosts` | Lister les machines |
| `services` | Lister les ports/services |
| `vulns` | Lister les vulnérabilités |
| `creds` | Lister les credentials |
| `loot` | Lister les fichiers récupérés |
| `notes` | Annotations sur les hosts |
| `hosts -R` | Charger les hosts dans RHOSTS |

---

## Leçons apprises

- Toujours créer un workspace avant de commencer — ça isole les données
- `db_nmap` remplace nmap : même syntaxe, données persistantes
- `services -R` permet de charger automatiquement les cibles dans les modules
- Les modules `post/gather` peuplent `loot` automatiquement
- La DB survit entre les sessions msfconsole — les données sont persistantes

## Références

- https://www.offsec.com/metasploit-unleashed/using-databases/
- https://www.rapid7.com/db/
