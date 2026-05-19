#!/bin/bash

service postgresql start

# Init si première fois, start si déjà initialisé
msfdb init 2>/dev/null || msfdb start

exec /bin/bash
