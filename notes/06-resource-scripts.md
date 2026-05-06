# Module 6 — Resource Scripts

## Description

Les resource scripts (.rc) permettent d'automatiser une séquence de
commandes Metasploit en une seule commande. C'est l'outil essentiel
pour structurer et reproduire des attaques.

## Usage

```bash
msfconsole -r script.rc
```

## auto_recon.rc

Script de reconnaissance automatique qui :
1. Scan nmap avec détection de versions
2. Scan TCP tous ports
3. Brute-force Telnet
4. Brute-force PostgreSQL

## Résultats obtenus

- 17 ports ouverts détectés
- Telnet : msfadmin:msfadmin
- PostgreSQL : postgres:postgres
- Session shell ouverte automatiquement
