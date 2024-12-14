/**
 * prefixeur.c
 * Programme pédagogique qui ajoute un préfixe à chaque ligne reçue sur l'entrée standard
 * Utilisé pour démontrer le fonctionnement des pipes sous Unix
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

// Taille maximale d'une ligne d'entrée
#define MAX_LIGNE 256

// Préfixe par défaut à ajouter devant chaque ligne
#define PREFIXE ">>> "

int main(void) {
    char ligne[MAX_LIGNE];
    int nombre_lignes = 0;
    
    // Désactive la mise en buffer pour un affichage immédiat
    setbuf(stdout, NULL);
    
    // Informe l'utilisateur que le programme est prêt
    fprintf(stderr, "Prefixeur démarré. Ajout du préfixe '%s' à chaque ligne...\n", PREFIXE);
    
    // Lit chaque ligne de l'entrée standard
    while (fgets(ligne, sizeof(ligne), stdin) != NULL) {
        nombre_lignes++;
        
        // Ajoute le préfixe et affiche la ligne sur stdout
        printf("%s%s", PREFIXE, ligne);
        
        // En cas d'erreur d'écriture
        if (ferror(stdout)) {
            fprintf(stderr, "Erreur lors de l'écriture : %s\n", strerror(errno));
            return 1;
        }
    }
    
    // Vérifie s'il y a eu une erreur de lecture
    if (ferror(stdin)) {
        fprintf(stderr, "Erreur lors de la lecture : %s\n", strerror(errno));
        return 1;
    }
    
    // Affiche un résumé sur stderr
    fprintf(stderr, "Fin du programme : %d lignes préfixées\n", nombre_lignes);
    
    return 0;
}