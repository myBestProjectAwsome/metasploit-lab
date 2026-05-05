# 🛡️ Metasploit Lab

Personal cybersecurity lab for learning Metasploit Framework in an isolated Docker environment.

## Architecture

| Container | Role | IP |
|---|---|---|
| Metasploitable 2 | Target | 192.168.100.2 |
| Kali Linux | Attacker | 192.168.100.3 |

Network: Docker bridge isolated (labnet — 192.168.100.0/24)

## Setup

### Requirements
- Ubuntu (host)
- Docker 20.x+

### Quick Start

```bash
# Créer le réseau isolé
docker network create --subnet=192.168.100.0/24 labnet

# Lancer Metasploitable
docker run -d \
  --name metasploitable \
  --network labnet \
  --ip 192.168.100.2 \
  --privileged \
  tleemcjr/metasploitable2 \
  /bin/sh -c "while true; do sleep 1; done"

# Démarrer les services dans Metasploitable
docker exec metasploitable service ssh start
docker exec metasploitable service apache2 start
docker exec metasploitable service vsftpd start
docker exec metasploitable service samba start

# Lancer Kali
docker run -it \
  --name kali \
  --network labnet \
  --ip 192.168.100.3 \
  kalilinux/kali-rolling /bin/bash

# Installer les outils dans Kali
apt update && apt install -y iputils-ping nmap metasploit-framework
```

## Exploits

| # | Vulnérabilité | CVE | Port | Status |
|---|---|---|---|---|
| 01 | Samba usermap_script | CVE-2007-2447 | 445 | ✅ |
| 02 | vsftpd 2.3.4 backdoor | CVE-2011-2523 | 21 | 🔜 |
| 03 | Distcc RCE | CVE-2004-2687 | 3632 | ✅ |
| 04 | PostgreSQL auth bypass | CVE-2007-3280 | 5432 | ✅ |

## Write-ups

- [01 — Samba usermap_script](notes/01-samba-exploit.md)
- [02 — Distcc RCE](notes/02-distcc-exploit.md)
- [03 — PostgreSQL Auth Bypass](notes/03-postgresql-exploit.md)

## ⚠️ Disclaimer

This lab is strictly for educational purposes in an isolated environment.
Never use these techniques on systems you don't own.

## Skills practiced
- Metasploit Framework (msfconsole)
- Network scanning (auxiliary modules)
- Exploitation (Samba, vsftpd...)
- Post-exploitation (shell, privilege escalation)
