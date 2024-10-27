#!/bin/bash

PROJECT_NAME="RedBerry"

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

error() {
    echo -e "${RED}Erreur: $1${NC}"
    exit 1
}

if ! command -v cmake &> /dev/null; then
    error "cmake n'est pas installé ou n'est pas dans le PATH"
fi

echo "Configuration du projet avec cmake..."
if ! cmake .; then
    error "Échec de la configuration avec cmake"
fi

echo "Compilation du projet..."
if ! make; then
    error "Échec de la compilation"
fi

EXECUTABLE="./$PROJECT_NAME"

if [ ! -f "$EXECUTABLE" ]; then
    error "L'exécutable $EXECUTABLE n'existe pas"
fi

echo "Ajout des permissions d'exécution..."
if ! chmod +x "$EXECUTABLE"; then
    error "Impossible d'ajouter les permissions d'exécution à $EXECUTABLE"
fi

echo -e "${GREEN}Exécution du projet...${NC}"
if ! "$EXECUTABLE"; then
    error "Échec de l'exécution du programme"
fi

echo -e "${GREEN}Exécution terminée avec succès${NC}"