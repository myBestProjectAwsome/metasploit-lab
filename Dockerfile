FROM kalilinux/kali-rolling

# Mise à jour et installation des outils
RUN apt update && apt install -y \
    metasploit-framework \
    iputils-ping \
    nmap \
    john \
    wordlists \
    curl \
    wget \
    netcat-traditional \
    python3 \
    python3-pip \
    openssh-client \
    && rm -rf /var/lib/apt/lists/*

# Décompresser rockyou.txt
RUN gunzip /usr/share/wordlists/rockyou.txt.gz 2>/dev/null || true

WORKDIR /root

CMD ["/bin/bash"]
