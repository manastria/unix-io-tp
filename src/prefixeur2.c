/**
 * prefixeur.c
 * Programme de démonstration de la manipulation des flux de texte sous Unix
 * 
 * Ce programme lit l'entrée standard ligne par ligne et ajoute un préfixe
 * configurable à chaque ligne. Il peut être utilisé dans une chaîne de pipes
 * pour formater ou annoter du texte.
 * 
 * Exemple d'utilisation :
 *     cat fichier.txt | ./prefixeur
 *     echo "test" | ./prefixeur -p "DEBUG: " -c cyan
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

// Configuration du programme
#define MAX_LIGNE 1024
#define MAX_PREFIX 50
#define MAX_HORODATAGE 30

// Codes de retour
#define SUCCESS 0
#define ERROR_ARGS 1
#define ERROR_LECTURE 2
#define ERROR_ECRITURE 3

// Codes couleur ANSI
#define COLOR_RESET   "\x1b[0m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"

// Structure de configuration
struct Config {
    char prefix[MAX_PREFIX];
    const char *color;
    int ajouter_horodatage;
    int numero_ligne;
};

/**
 * Initialise la configuration par défaut
 */
void init_config(struct Config *config) {
    strcpy(config->prefix, ">> ");
    config->color = COLOR_RESET;
    config->ajouter_horodatage = 0;
    config->numero_ligne = 0;
}

/**
 * Génère l'horodatage au format [HH:MM:SS]
 */
void obtenir_horodatage(char *buffer, size_t taille) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, taille, "[%H:%M:%S]", tm_info);
}

/**
 * Affiche l'aide du programme
 */
void afficher_aide(const char *nom_programme) {
    fprintf(stderr, "Usage: %s [options]\n\n", nom_programme);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -p PREFIX   Définit le préfixe (défaut: '>> ')\n");
    fprintf(stderr, "  -c COULEUR  Définit la couleur (red, green, blue, yellow, magenta, cyan)\n");
    fprintf(stderr, "  -t          Ajoute un horodatage\n");
    fprintf(stderr, "  -n          Ajoute la numérotation des lignes\n");
    fprintf(stderr, "  -h          Affiche cette aide\n");
}

/**
 * Définit la couleur en fonction de l'argument
 */
const char* obtenir_couleur(const char *nom_couleur) {
    if (!nom_couleur) return COLOR_RESET;
    
    if (strcmp(nom_couleur, "red") == 0)     return COLOR_RED;
    if (strcmp(nom_couleur, "green") == 0)   return COLOR_GREEN;
    if (strcmp(nom_couleur, "blue") == 0)    return COLOR_BLUE;
    if (strcmp(nom_couleur, "yellow") == 0)  return COLOR_YELLOW;
    if (strcmp(nom_couleur, "magenta") == 0) return COLOR_MAGENTA;
    if (strcmp(nom_couleur, "cyan") == 0)    return COLOR_CYAN;
    
    fprintf(stderr, "Attention: couleur '%s' inconnue, utilisation de la couleur par défaut\n", 
            nom_couleur);
    return COLOR_RESET;
}

/**
 * Parse les arguments de la ligne de commande
 */
int parser_arguments(int argc, char *argv[], struct Config *config) {
    int opt;
    while ((opt = getopt(argc, argv, "hp:c:tn")) != -1) {
        switch (opt) {
            case 'h':
                afficher_aide(argv[0]);
                exit(SUCCESS);
            case 'p':
                if (strlen(optarg) < MAX_PREFIX) {
                    strcpy(config->prefix, optarg);
                } else {
                    fprintf(stderr, "Erreur: préfixe trop long (max %d caractères)\n", 
                            MAX_PREFIX - 1);
                    return ERROR_ARGS;
                }
                break;
            case 'c':
                config->color = obtenir_couleur(optarg);
                break;
            case 't':
                config->ajouter_horodatage = 1;
                break;
            case 'n':
                config->numero_ligne = 1;
                break;
            default:
                afficher_aide(argv[0]);
                return ERROR_ARGS;
        }
    }
    return SUCCESS;
}

/**
 * Traite l'entrée standard ligne par ligne
 */
int traiter_entree(struct Config *config) {
    char ligne[MAX_LIGNE];
    char horodatage[MAX_HORODATAGE];
    int numero = 0;
    
    // Traite chaque ligne de l'entrée
    while (fgets(ligne, sizeof(ligne), stdin) != NULL) {
        numero++;
        
        // Génère l'horodatage si nécessaire
        if (config->ajouter_horodatage) {
            obtenir_horodatage(horodatage, sizeof(horodatage));
        }
        
        // Écrit la ligne avec le préfixe et les options demandées
        if (fprintf(stdout, "%s", config->color) < 0) goto error_ecriture;
        
        if (config->numero_ligne) {
            if (fprintf(stdout, "%4d ", numero) < 0) goto error_ecriture;
        }
        
        if (config->ajouter_horodatage) {
            if (fprintf(stdout, "%s ", horodatage) < 0) goto error_ecriture;
        }
        
        if (fprintf(stdout, "%s%s%s", config->prefix, ligne, COLOR_RESET) < 0) {
            goto error_ecriture;
        }
        
        // Force l'écriture immédiate
        fflush(stdout);
    }
    
    // Vérifie les erreurs de lecture
    if (ferror(stdin)) {
        fprintf(stderr, "Erreur de lecture : %s\n", strerror(errno));
        return ERROR_LECTURE;
    }
    
    return SUCCESS;

error_ecriture:
    fprintf(stderr, "Erreur d'écriture : %s\n", strerror(errno));
    return ERROR_ECRITURE;
}

/**
 * Fonction principale
 */
int main(int argc, char *argv[]) {
    struct Config config;
    init_config(&config);
    
    // Parse les arguments
    int resultat = parser_arguments(argc, argv, &config);
    if (resultat != SUCCESS) return resultat;
    
    // Traite l'entrée
    resultat = traiter_entree(&config);
    
    // Message de fin sur stderr
    fprintf(stderr, "Programme terminé avec code : %d\n", resultat);
    
    return resultat;
}