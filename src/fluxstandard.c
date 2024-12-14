/**
 * fluxstandard.c
 * Programme pédagogique pour démontrer les flux standards sous Unix
 * 
 * Options :
 *   -c    Active la colorisation des sorties (uniquement sur terminal)
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <stdarg.h>
#include <getopt.h>

// Taille maximale d'une ligne d'entrée
#define MAX_LIGNE 256

// Codes couleur ANSI
#define COULEUR_STDOUT    "\033[32m"   // Vert pour stdout
#define COULEUR_STDERR    "\033[31m"   // Rouge pour stderr
#define COULEUR_STDIN     "\033[36m"   // Cyan pour stdin
#define COULEUR_RESET     "\033[0m"

// Structure pour gérer la colorisation
struct Config {
    int utiliser_couleur;     // Option -c activée
    int stdout_est_tty;       // stdout est un terminal
    int stderr_est_tty;       // stderr est un terminal
};

// Initialise la configuration en vérifiant l'environnement
void init_config(struct Config *cfg) {
    cfg->utiliser_couleur = 0;
    cfg->stdout_est_tty = isatty(STDOUT_FILENO);
    cfg->stderr_est_tty = isatty(STDERR_FILENO);
}

// Parse les arguments de la ligne de commande
void parser_arguments(int argc, char *argv[], struct Config *cfg) {
    int opt;
    while ((opt = getopt(argc, argv, "c")) != -1) {
        switch (opt) {
            case 'c':
                cfg->utiliser_couleur = 1;
                break;
        }
    }
}

// Vérifie si des données sont disponibles sur stdin
int donnees_disponibles(void) {
    fd_set fds;
    struct timeval tv;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
}

// Fonctions d'affichage avec gestion de la couleur
void afficher_stdout(const struct Config *cfg, const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    if (cfg->utiliser_couleur && cfg->stdout_est_tty) {
        printf("%s", COULEUR_STDOUT);
    }
    
    vprintf(format, args);
    
    if (cfg->utiliser_couleur && cfg->stdout_est_tty) {
        printf("%s", COULEUR_RESET);
    }
    
    va_end(args);
}

void afficher_stderr(const struct Config *cfg, const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    if (cfg->utiliser_couleur && cfg->stderr_est_tty) {
        fprintf(stderr, "%s", COULEUR_STDERR);
    }
    
    vfprintf(stderr, format, args);
    
    if (cfg->utiliser_couleur && cfg->stderr_est_tty) {
        fprintf(stderr, "%s", COULEUR_RESET);
    }
    
    va_end(args);
}

int main(int argc, char *argv[]) {
    char ligne[MAX_LIGNE];
    struct Config cfg;
    
    // Initialisation
    init_config(&cfg);
    parser_arguments(argc, argv, &cfg);
    
    // Désactive la mise en buffer pour voir les messages immédiatement
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    // Messages de démonstration
    afficher_stdout(&cfg, "Message normal 1 : Bienvenue !\n");
    afficher_stderr(&cfg, "Message d'erreur 1 : Un problème est survenu\n");
    usleep(100000);

    afficher_stderr(&cfg, "Message d'erreur 2 : Attention !\n");
    afficher_stdout(&cfg, "Message normal 2 : Le programme continue...\n");
    usleep(100000);

    afficher_stdout(&cfg, "Message normal 3 : Presque terminé\n");
    afficher_stderr(&cfg, "Message d'erreur 3 : Dernier avertissement\n");
    usleep(100000);

    // Vérification et traitement de l'entrée standard
    if (donnees_disponibles()) {
        afficher_stdout(&cfg, "\n=== Des données sont disponibles sur l'entrée standard ===\n");
        afficher_stderr(&cfg, "Lecture des données...\n");
        
        while (fgets(ligne, sizeof(ligne), stdin) != NULL) {
            size_t len = strlen(ligne);
            if (len > 0 && ligne[len-1] == '\n') {
                ligne[len-1] = '\0';
            }
            
            if (cfg.utiliser_couleur && cfg.stdout_est_tty) {
                printf("%sLu sur stdin : '%s'%s\n", 
                       COULEUR_STDIN, ligne, COULEUR_RESET);
            } else {
                printf("Lu sur stdin : '%s'\n", ligne);
            }
            
            afficher_stderr(&cfg, "Reçu une ligne de %zu caractères\n", len);
        }
    }

    // Message de fin
    afficher_stdout(&cfg, "\nMessage normal 4 : Programme terminé avec succès\n");

    return 0;
}