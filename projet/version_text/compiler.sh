#!/bin/bash

# Compilation du serveur
gcc server.c -o server

# Vérification si la compilation du serveur a réussi
if [ $? -eq 0 ]; then
    echo "Compilation du serveur réussie."
else
    echo "Erreur lors de la compilation du serveur."
    exit 1
fi

# Compilation du client
gcc client.c -o client

# Vérification si la compilation du client a réussi
if [ $? -eq 0 ]; then
    echo "Compilation du client réussie."
else
    echo "Erreur lors de la compilation du client."
    exit 1
fi

echo "Les exécutables 'server' et 'client' ont été créés avec succès."
