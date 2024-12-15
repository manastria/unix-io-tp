#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Structures
typedef struct {
    int connexion_echouee;
    time_t derniere_action;
    int est_connecte;
    char derniere_ip[16];
    int actions_suspectes;
} UserState;

typedef struct {
    int heure_debut;
    int heure_fin;
    const char* attaquant;
    const char* cible;
    const char* type_attaque;
} Scenario;

typedef struct {
    int heure;
    int probabilite;  // Sur 100
} Distribution;

// Distribution temporelle des événements
const Distribution DISTRIBUTION[] = {
    {0, 5},   {1, 3},   {2, 2},   {3, 2},   {4, 3},   {5, 5},
    {6, 20},  {7, 40},  {8, 80},  {9, 100}, {10, 90}, {11, 85},
    {12, 60}, {13, 65}, {14, 95}, {15, 90}, {16, 85}, {17, 70},
    {18, 40}, {19, 20}, {20, 15}, {21, 10}, {22, 8},  {23, 6}
};

// Configuration globale
const char* USERS[] = {"alice", "bob", "charlie", "david", "eve", "admin"};
const int NB_USERS = 6;

const char* ACTIONS[] = {
    "LOGIN", "LOGOUT", "UPLOAD", "DOWNLOAD", 
    "VIEW", "MODIFY", "DELETE", "EXECUTE"
};
const int NB_ACTIONS = 8;

const char* FILES[] = {
    "rapport_financier.pdf", "config.xml", "backup.zip",
    "utilisateurs.db", "script.sh", "donnees.csv",
    "/etc/passwd", "/etc/shadow", "index.html",
    ".bashrc", "id_rsa", "access.log"
};
const int NB_FILES = 12;

const char* IP_PREFIXES[] = {"192.168.1.", "10.0.0.", "172.16.0.", "8.8.8."};
const int NB_IP_PREFIXES = 4;

const char* STATUS[] = {"SUCCESS", "ERROR", "DENIED", "TIMEOUT"};
const int NB_STATUS = 4;

// Fonctions utilitaires
void generer_ip(char* ip, int force_externe) {
    int prefix = force_externe ? 3 : (rand() % (NB_IP_PREFIXES - 1));
    sprintf(ip, "%s%d", IP_PREFIXES[prefix], rand() % 255);
}

void generer_timestamp(char* timestamp, int heure, int minute, int seconde) {
    sprintf(timestamp, "[%02d:%02d:%02d]", heure, minute, seconde);
}

// Fonction pour générer un événement normal
void generer_evenement(UserState* etats, int heure, int minute, int mode_realiste) {
    char timestamp[20];
    char ip[16];
    int user_idx = rand() % (NB_USERS - 1);  // Exclut eve des événements normaux
    int action_idx = rand() % NB_ACTIONS;
    int file_idx = rand() % NB_FILES;
    int status_idx;
    
    generer_ip(ip, 0);
    generer_timestamp(timestamp, heure, minute, rand() % 60);
    
    // Gestion réaliste des statuts
    if (mode_realiste) {
        if (strcmp(ACTIONS[action_idx], "LOGIN") == 0) {
            status_idx = (rand() % 100 < 20) ? 1 : 0;  // 20% d'échec pour login
        } else {
            status_idx = (rand() % 100 < 5) ? 1 : 0;   // 5% d'échec pour le reste
        }
    } else {
        status_idx = rand() % NB_STATUS;
    }
    
    // Mise à jour de l'état de l'utilisateur
    if (strcmp(ACTIONS[action_idx], "LOGIN") == 0) {
        if (status_idx != 0) {
            etats[user_idx].connexion_echouee++;
            if (etats[user_idx].connexion_echouee >= 3) {
                fprintf(stderr, "Alert: Multiple failed login attempts for %s from %s\n",
                        USERS[user_idx], ip);
                fflush(stderr);
            }
        } else {
            etats[user_idx].est_connecte = 1;
            etats[user_idx].connexion_echouee = 0;
        }
    }
    
    printf("%s %s %s %s %s %s\n",
           timestamp, ip, USERS[user_idx],
           ACTIONS[action_idx], FILES[file_idx],
           STATUS[status_idx]);
    
    fflush(stdout);

    etats[user_idx].derniere_action = time(NULL);
    strcpy(etats[user_idx].derniere_ip, ip);
}

// Fonction pour générer une attaque d'eve
void generer_attaque_eve(int heure, int minute) {
    char ip[16];
    generer_ip(ip, 1);  // Force une IP externe
    
    for (int i = 0; i < 5; i++) {
        char timestamp[20];
        generer_timestamp(timestamp, heure, minute, i * 10);
        
        printf("%s %s eve LOGIN %s %s\n",
               timestamp, ip,
               FILES[rand() % NB_FILES],
               STATUS[rand() % 10 == 0 ? 0 : 2]);  // 10% de succès
        fflush(stdout);

        if (i == 4) {
            fprintf(stderr, "ALERT [%02d:%02d:%02d] Multiple failed login attempts from %s\n",
                    heure, minute, i * 10, ip);
            fflush(stderr);
        }

        
    }
}

// Fonction pour générer un scénario d'attaque
void generer_scenario_attaque(const Scenario* scenario) {
    char ip[16];
    generer_ip(ip, 1);
    
    // Phase 1: Reconnaissance
    for (int minute = 0; minute < 5; minute++) {
        char timestamp[20];
        generer_timestamp(timestamp, scenario->heure_debut, minute, 0);
        
        printf("%s %s %s LOGIN %s ERROR\n",
               timestamp, ip, scenario->attaquant,
               FILES[rand() % NB_FILES]);
        fflush(stdout);
    }
    
    // Phase 2: Exploitation
    char timestamp[20];
    generer_timestamp(timestamp, scenario->heure_debut + 1, 0, 0);
    printf("%s %s %s LOGIN /etc/passwd SUCCESS\n",
           timestamp, ip, scenario->attaquant);
    fflush(stdout);

    // Phase 3: Actions malveillantes
    const char* actions_malveillantes[] = {
        "VIEW /etc/passwd",
        "VIEW /etc/shadow",
        "DOWNLOAD utilisateurs.db",
        "MODIFY config.xml"
    };
    
    for (int i = 0; i < 4; i++) {
        generer_timestamp(timestamp, scenario->heure_debut + 1, i * 15, 0);
        printf("%s %s %s %s SUCCESS\n",
               timestamp, ip, scenario->attaquant,
               actions_malveillantes[i]);
        fflush(stdout);
    }
    
    fprintf(stderr, "ALERT: Possible intrusion detected from %s\n", ip);
    fprintf(stderr, "ALERT: Sensitive files accessed by %s\n", scenario->attaquant);
    fflush(stderr);
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    UserState* etats_utilisateurs = calloc(NB_USERS, sizeof(UserState));
    
    // Analyse des arguments
    int mode_realtime = 0;
    int mode_scenario = 0;
    int mode_chronologique = 1;  // Mode par défaut
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--real-time") == 0) {
            mode_realtime = 1;
            mode_chronologique = 0;
        }
        if (strcmp(argv[i], "--scenario") == 0) {
            mode_scenario = 1;
            mode_chronologique = 0;
        }
    }
    
    // Mode scénario
    if (mode_scenario) {
        Scenario scenario = {
            .heure_debut = 23,
            .heure_fin = 4,
            .attaquant = "eve",
            .cible = "admin",
            .type_attaque = "bruteforce"
        };
        generer_scenario_attaque(&scenario);
        free(etats_utilisateurs);
        return 0;
    }
    
    // Mode chronologique (par défaut)
    if (mode_chronologique) {
        for (int heure = 0; heure < 24; heure++) {
            int events_par_minute = DISTRIBUTION[heure].probabilite / 20 + 1;
            
            for (int minute = 0; minute < 60; minute++) {
                for (int e = 0; e < events_par_minute; e++) {
                    generer_evenement(etats_utilisateurs, heure, minute, 1);
                }
                
                if (rand() % 100 < 2) {  // 2% de chance d'attaque par eve
                    generer_attaque_eve(heure, minute);
                }
            }
        }
    }
    
    // Mode temps réel
    if (mode_realtime) {
        while (1) {
            time_t now = time(NULL);
            struct tm *tm_info = localtime(&now);
            
            generer_evenement(etats_utilisateurs, 
                            tm_info->tm_hour,
                            tm_info->tm_min, 1);
            
            if (rand() % 100 < 2) {
                generer_attaque_eve(tm_info->tm_hour, tm_info->tm_min);
            }
            
            usleep(rand() % 1000000);  // 0-1 seconde entre les événements
        }
    }
    
    free(etats_utilisateurs);
    return 0;
}