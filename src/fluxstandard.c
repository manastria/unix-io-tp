/**
 * fluxstandard.c
 * Programme pédagogique pour démontrer les flux standards sous Unix
 */

#include <stdio.h>
#include <unistd.h>

int main(void) {
    // Désactive la mise en buffer pour voir les messages immédiatement
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    // Première série de messages
    printf("Message normal 1 : Bienvenue !\n");
    fprintf(stderr, "Message d'erreur 1 : Un problème est survenu\n");

    // Courte pause pour s'assurer que les messages précédents sont affichés
    usleep(100000);  // 0.1 seconde

    // Deuxième série de messages
    fprintf(stderr, "Message d'erreur 2 : Attention !\n");
    printf("Message normal 2 : Le programme continue...\n");

    usleep(100000);

    // Troisième série de messages
    printf("Message normal 3 : Presque terminé\n");
    fprintf(stderr, "Message d'erreur 3 : Dernier avertissement\n");

    usleep(100000);

    // Message de fin
    printf("Message normal 4 : Programme terminé avec succès\n");

    return 0;
}