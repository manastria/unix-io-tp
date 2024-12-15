#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Structure pour la distribution horaire
typedef struct {
    int heure;
    int probabilite;  // Sur 100
} Distribution;

// Distribution normale des événements sur 24h
const Distribution DISTRIBUTION[] = {
    {0, 5},   {1, 3},   {2, 2},   {3, 2},   {4, 3},   {5, 5},
    {6, 20},  {7, 40},  {8, 80},  {9, 100}, {10, 90}, {11, 85},
    {12, 60}, {13, 65}, {14, 95}, {15, 90}, {16, 85}, {17, 70},
    {18, 40}, {19, 20}, {20, 15}, {21, 10}, {22, 8},  {23, 6}
};

// Configuration des utilisateurs et actions
const char* USERS[] = {"alice", "bob", "charlie", "david", "eve", "admin"};
const char* ACTIONS[] = {"LOGIN", "LOGOUT", "UPLOAD", "DOWNLOAD", "VIEW", "MODIFY", "DELETE"};
const char* FILES[] = {
    "rapport_financier.pdf", "config.xml", "backup.zip",
    "utilisateurs.db", "script.sh", "donnees.csv",
    "/etc/passwd", "/etc/shadow", "index.html"
};
const char* IP_PREFIXES[] = {"192.168.1.", "10.0.0.", "172.16.0.", "8.8.8."};
const char* STATUS[] = {"SUCCESS", "ERROR", "DENIED"};

// Fonction pour générer un événement normal
void generer_evenement(int heure, int minute) {
    char ip[16];
    sprintf(ip, "%s%d", IP_PREFIXES[rand() % 4], rand() % 255);
    
    printf("[%02d:%02d:00] %s %s %s %s %s\n",
           heure, minute,
           ip,
           USERS[rand() % 5],  // eve n'est pas incluse dans les événements normaux
           ACTIONS[rand() % 7],
           FILES[rand() % 9],
           STATUS[rand() % 2 == 0 ? 0 : 1]  // 50% de succès
    );
}

// Fonction pour générer une tentative d'attaque par eve
void generer_attaque_eve(int heure, int minute) {
    char ip[16];
    sprintf(ip, "8.8.8.%d", rand() % 255);  // IP externe pour eve
    
    // Génère 5 tentatives rapides
    for (int i = 0; i < 5; i++) {
        printf("[%02d:%02d:%02d] %s eve LOGIN %s %s\n",
               heure, minute, i * 10,  // Espacées de 10 secondes
               ip,
               FILES[rand() % 9],
               STATUS[rand() % 10 == 0 ? 0 : 2]  // 10% de succès
        );
        
        if (i == 4) {  // Alerte après 5 tentatives
            fprintf(stderr, "ALERT [%02d:%02d:%02d] Multiple failed login attempts from %s\n",
                    heure, minute, i * 10, ip);
        }
    }
}

int main() {
    srand(time(NULL));
    
    // Génère des événements pour chaque minute de la journée
    for (int heure = 0; heure < 24; heure++) {
        int events_par_minute = DISTRIBUTION[heure].probabilite / 20 + 1;
        
        for (int minute = 0; minute < 60; minute++) {
            // Événements normaux
            for (int e = 0; e < events_par_minute; e++) {
                generer_evenement(heure, minute);
            }
            
            // Tentatives d'attaque d'eve (2% de chance par minute)
            if (rand() % 100 < 2) {
                generer_attaque_eve(heure, minute);
            }
        }
    }
    
    return 0;
}