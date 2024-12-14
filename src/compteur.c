/**
 * compteur.c
 * Programme de démonstration du traitement des flux d'entrée sous Unix
 * 
 * Ce programme lit l'entrée standard ligne par ligne, numérote chaque ligne,
 * et maintient des statistiques qu'il affiche sur stderr.
 * Il est conçu pour être utilisé dans des pipes Unix, par exemple :
 *     cat fichier.txt | ./compteur | grep "mot"
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>

// Constantes pour la configuration du programme
#define MAX_LIGNE 1024       // Longueur maximale d'une ligne
#define MAX_MOTS 100        // Nombre maximum de mots par ligne
#define MIN_MOT_LEN 2       // Longueur minimale pour compter un mot

// Codes de retour du programme
#define SUCCESS 0
#define ERROR_LECTURE 1
#define ERROR_ECRITURE 2

// Structure pour stocker les statistiques
struct Statistiques {
    int nombre_lignes;
    int nombre_mots;
    int nombre_caracteres;
    int plus_longue_ligne;
};

/**
 * Initialise la structure de statistiques
 */
void init_statistiques(struct Statistiques *stats) {
    stats->nombre_lignes = 0;
    stats->nombre_mots = 0;
    stats->nombre_caracteres = 0;
    stats->plus_longue_ligne = 0;
}

/**
 * Compte le nombre de mots dans une ligne
 * Un mot est défini comme une séquence de caractères non-blancs
 * de longueur minimale MIN_MOT_LEN
 */
int compter_mots(const char *ligne) {
    int mots = 0;
    int len_mot = 0;
    int dans_mot = 0;

    for (int i = 0; ligne[i] != '\0'; i++) {
        if (isspace(ligne[i])) {
            if (dans_mot && len_mot >= MIN_MOT_LEN) {
                mots++;
            }
            dans_mot = 0;
            len_mot = 0;
        } else {
            dans_mot = 1;
            len_mot++;
        }
    }
    // Vérifie le dernier mot
    if (dans_mot && len_mot >= MIN_MOT_LEN) {
        mots++;
    }
    
    return mots;
}

/**
 * Met à jour les statistiques pour une ligne donnée
 */
void mettre_a_jour_statistiques(struct Statistiques *stats, const char *ligne) {
    stats->nombre_lignes++;
    
    int longueur = strlen(ligne);
    stats->nombre_caracteres += longueur;
    
    if (longueur > stats->plus_longue_ligne) {
        stats->plus_longue_ligne = longueur;
    }
    
    stats->nombre_mots += compter_mots(ligne);
}

/**
 * Affiche les statistiques sur stderr
 */
void afficher_statistiques(const struct Statistiques *stats) {
    fprintf(stderr, "\n=== Statistiques ===\n");
    fprintf(stderr, "Nombre de lignes : %d\n", stats->nombre_lignes);
    fprintf(stderr, "Nombre de mots : %d\n", stats->nombre_mots);
    fprintf(stderr, "Nombre de caractères : %d\n", stats->nombre_caracteres);
    fprintf(stderr, "Longueur de la plus longue ligne : %d\n", stats->plus_longue_ligne);
    fprintf(stderr, "Moyenne de mots par ligne : %.2f\n",
            stats->nombre_lignes > 0 ? (float)stats->nombre_mots / stats->nombre_lignes : 0);
}

/**
 * Traite l'entrée standard ligne par ligne
 * Retourne 0 en cas de succès, un code d'erreur sinon
 */
int traiter_entree(void) {
    char ligne[MAX_LIGNE];
    struct Statistiques stats;
    init_statistiques(&stats);
    
    // Lit et traite chaque ligne de l'entrée standard
    while (fgets(ligne, sizeof(ligne), stdin) != NULL) {
        // Met à jour les statistiques
        mettre_a_jour_statistiques(&stats, ligne);
        
        // Affiche la ligne numérotée sur stdout
        if (fprintf(stdout, "%4d | %s", stats.nombre_lignes, ligne) < 0) {
            fprintf(stderr, "Erreur d'écriture : %s\n", strerror(errno));
            return ERROR_ECRITURE;
        }
        
        // Force l'écriture immédiate
        fflush(stdout);
    }
    
    // Vérifie si la lecture s'est terminée à cause d'une erreur
    if (ferror(stdin)) {
        fprintf(stderr, "Erreur de lecture : %s\n", strerror(errno));
        return ERROR_LECTURE;
    }
    
    // Affiche les statistiques finales
    afficher_statistiques(&stats);
    
    return SUCCESS;
}

/**
 * Fonction principale
 */
int main(void) {
    // Message de démarrage sur stderr
    fprintf(stderr, "Démarrage du compteur...\n");
    
    // Traite l'entrée
    int resultat = traiter_entree();
    
    // Message de fin sur stderr
    fprintf(stderr, "Programme terminé avec code : %d\n", resultat);
    
    return resultat;
}