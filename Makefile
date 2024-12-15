# Définition des options de compilation
CC = gcc
# -Wall active tous les avertissements
# -Wextra ajoute des avertissements supplémentaires
# -g ajoute les informations de débogage
CFLAGS = -Wall -Wextra -g

# Définition des programmes à compiler
PROGRAMS = compteur prefixeur base_io fluxstandard numeroteur loggen

# La première cible est celle par défaut (exécutée quand on tape juste 'make')
all: $(PROGRAMS)

# Règle générique pour compiler les programmes
# $@ : correspond au nom de la cible
# $< : correspond à la première dépendance
%: src/%.c
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

# Ajouter les exécutables générés dans .gitignore
update-gitignore:
	@for prog in $(PROGRAMS); do \
		if ! grep -q "^$$prog$$" .gitignore; then \
			echo $$prog >> .gitignore; \
			echo "Ajouté $$prog à .gitignore"; \
		fi \
	done

# Indique que ces cibles ne correspondent pas à des fichiers
.PHONY: all clean rebuild prepare-test test install