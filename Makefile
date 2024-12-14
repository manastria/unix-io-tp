# Définition des options de compilation
CC = gcc
# -Wall active tous les avertissements
# -Wextra ajoute des avertissements supplémentaires
# -g ajoute les informations de débogage
CFLAGS = -Wall -Wextra -g

# Définition des programmes à compiler
PROGRAMS = compteur prefixeur base_io fluxstandard numeroteur 

# La première cible est celle par défaut (exécutée quand on tape juste 'make')
all: $(PROGRAMS)

# Règles de compilation pour chaque programme
prog1: src/base_io.c
	$(CC) $(CFLAGS) -o $@ $<

compteur: src/compteur.c
	$(CC) $(CFLAGS) -o $@ $<

prefixeur: src/prefixeur.c
	$(CC) $(CFLAGS) -o $@ $<

# Cible pour nettoyer les fichiers générés
clean:
	rm -f $(PROGRAMS) *.txt

# Cible pour nettoyer et recompiler
rebuild: clean all

# Cible pour créer les fichiers de test nécessaires
prepare-test:
	echo "Première ligne" > entree.txt
	echo "Deuxième ligne" >> entree.txt
	echo "Troisième ligne" >> entree.txt

# Cible pour exécuter des tests de base
test: all prepare-test
	@echo "=== Test de base_io ==="
	./prog1 > sortie.txt 2> erreurs.txt
	@echo "Contenu de sortie.txt:"
	@cat sortie.txt
	@echo "\nContenu de erreurs.txt:"
	@cat erreurs.txt
	
	@echo "\n=== Test du compteur ==="
	cat entree.txt | ./compteur
	
	@echo "\n=== Test du prefixeur ==="
	cat entree.txt | ./prefixeur

# Cible pour installer (copier les exécutables dans ~/bin)
install: all
	mkdir -p ~/bin
	cp $(PROGRAMS) ~/bin/

# Indique que ces cibles ne correspondent pas à des fichiers
.PHONY: all clean rebuild prepare-test test install