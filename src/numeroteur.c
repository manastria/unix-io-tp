/**
 * numeroteur.c
 * Programme pédagogique qui numérote les lignes reçues sur son entrée standard
 * Utilisé pour démontrer le fonctionnement des pipes sous Unix
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

// Taille maximale d'une ligne d'entrée
#define MAX_LIGNE 256

int main(void) {
    char ligne[MAX_LIGNE];
    int numero = 0;
    
    // Désactive la mise en buffer pour un affichage immédiat
    setbuf(stdout, NULL);
    
    // Informe l'utilisateur que le programme est prêt
    fprintf(stderr, "Numeroteur démarré. En attente de lignes...\n");
    
    // Lit chaque ligne de l'entrée standard
    while (fgets(ligne, sizeof(ligne), stdin) != NULL) {
        numero++;
        
        // Affiche la ligne avec son numéro sur stdout
        printf("%3d | %s", numero, ligne);
        
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
    fprintf(stderr, "Fin du programme : %d lignes traitées\n", numero);
    
    return 0;
}