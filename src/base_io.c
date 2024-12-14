/**
 * base_io.c
 * Programme de démonstration des entrées/sorties standard sous Unix
 * 
 * Ce programme illustre les trois flux standard :
 * - stdin  : l'entrée standard (lecture)
 * - stdout : la sortie standard (affichage normal)
 * - stderr : la sortie d'erreur (messages d'erreur)
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

// Taille maximale d'une ligne lue depuis l'entrée standard
#define MAX_LIGNE 256

// Codes de retour du programme
#define SUCCESS 0
#define ERROR_LECTURE 1

/**
 * Affiche un message de bienvenue sur la sortie standard et d'erreur
 * Cette fonction permet de voir clairement la différence entre stdout et stderr
 */
void afficher_messages_demo(void) {
    // Messages sur la sortie standard (stdout)
    printf("=== Messages sur stdout ===\n");
    printf("1. Ceci est un message normal\n");
    printf("2. Les messages stdout sont pour les sorties normales du programme\n");
    
    // Messages sur la sortie d'erreur (stderr)
    fprintf(stderr, "=== Messages sur stderr ===\n");
    fprintf(stderr, "1. Ceci est un message d'erreur\n");
    fprintf(stderr, "2. Les messages stderr sont pour les erreurs et diagnostics\n");
}

/**
 * Lit l'entrée standard ligne par ligne et affiche chaque ligne
 * Retourne 0 en cas de succès, 1 en cas d'erreur
 */
int traiter_entree_standard(void) {
    char ligne[MAX_LIGNE];
    int numero_ligne = 0;
    
    printf("\n=== Lecture de stdin ===\n");
    fprintf(stderr, "Début de la lecture de stdin...\n");
    
    // Lecture ligne par ligne jusqu'à la fin de l'entrée (Ctrl+D)
    while (fgets(ligne, sizeof(ligne), stdin) != NULL) {
        numero_ligne++;
        
        // Supprime le retour à la ligne final s'il existe
        size_t len = strlen(ligne);
        if (len > 0 && ligne[len-1] == '\n') {
            ligne[len-1] = '\0';
        }
        
        // Affiche la ligne lue sur stdout
        printf("Ligne %d : %s\n", numero_ligne, ligne);
    }
    
    // Vérifie si la lecture s'est terminée à cause d'une erreur
    if (ferror(stdin)) {
        fprintf(stderr, "Erreur lors de la lecture : %s\n", strerror(errno));
        return ERROR_LECTURE;
    }
    
    // Affiche un résumé sur stderr
    fprintf(stderr, "Fin de la lecture : %d lignes lues\n", numero_ligne);
    return SUCCESS;
}

/**
 * Fonction principale
 */
int main(void) {
    // Désactive la mise en buffer de stdout pour voir les messages immédiatement
    setbuf(stdout, NULL);
    
    // Phase 1 : Démonstration des messages stdout/stderr
    afficher_messages_demo();
    
    // Phase 2 : Traitement de l'entrée standard
    int resultat = traiter_entree_standard();
    
    // Message de fin sur stderr
    fprintf(stderr, "\nProgramme terminé avec code : %d\n", resultat);
    
    return resultat;
}