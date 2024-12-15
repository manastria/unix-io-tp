#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Structure pour suivre l'état des utilisateurs
typedef struct {
    int connexion_echouee;
    time_t derniere_action;
    int est_connecte;
    char derniere_ip[16];
} UserState;

// Structure pour définir un scénario d'attaque
typedef struct {
    int heure_debut;
    int heure_fin;
    const char* attaquant;
    const char* cible;
    const char* type_attaque;
} Scenario;

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

// Fonction pour générer une adresse IP aléatoire
void generer_ip(char* ip) {
    int prefix = rand() % NB_IP_PREFIXES;
    int suffix = rand() % 255;
    sprintf(ip, "%s%d", IP_PREFIXES[prefix], suffix);
}

// Fonction pour générer un timestamp dans le format [HH:MM:SS]
void generer_timestamp(char* timestamp, int heure_forcee) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    if (heure_forcee >= 0) {
        tm_info->tm_hour = heure_forcee;
    }
    
    sprintf(timestamp, "[%02d:%02d:%02d]", 
            tm_info->tm_hour,
            tm_info->tm_min, 
            tm_info->tm_sec);
}

// Fonction pour générer un événement de log basique
void generer_evenement(UserState* etats, int mode_realiste) {
    char timestamp[20];
    char ip[16];
    int user_idx = rand() % NB_USERS;
    int action_idx = rand() % NB_ACTIONS;
    int file_idx = rand() % NB_FILES;
    int status_idx;
    
    // Génération plus réaliste des statuts
    if (mode_realiste) {
        if (strcmp(ACTIONS[action_idx], "LOGIN") == 0) {
            // 20% de chance d'échec pour les logins
            status_idx = (rand() % 100 < 20) ? 1 : 0;
        } else {
            // 5% de chance d'erreur pour les autres actions
            status_idx = (rand() % 100 < 5) ? 1 : 0;
        }
    } else {
        status_idx = rand() % NB_STATUS;
    }
    
    generer_timestamp(timestamp, -1);
    generer_ip(ip);
    
    // Mise à jour de l'état de l'utilisateur
    if (strcmp(ACTIONS[action_idx], "LOGIN") == 0) {
        if (status_idx != 0) {
            etats[user_idx].connexion_echouee++;
            if (etats[user_idx].connexion_echouee >= 3) {
                fprintf(stderr, "Alert: Multiple failed login attempts for %s from %s\n", 
                        USERS[user_idx], ip);
            }
        } else {
            etats[user_idx].est_connecte = 1;
            etats[user_idx].connexion_echouee = 0;
        }
    }
    
    // Génération de l'entrée de log
    printf("%s %s %s %s %s %s\n",
           timestamp, ip, USERS[user_idx],
           ACTIONS[action_idx], FILES[file_idx],
           STATUS[status_idx]);
    
    // Mise à jour du timestamp de dernière action
    etats[user_idx].derniere_action = time(NULL);
    strcpy(etats[user_idx].derniere_ip, ip);
}

// Fonction pour générer un scénario d'attaque
void generer_scenario_attaque(const Scenario* scenario) {
    char timestamp[20];
    char ip[16] = "8.8.8.8"; // IP externe pour l'attaque
    
    // Phase 1: Reconnaissance (tentatives de connexion)
    for (int i = 0; i < 5; i++) {
        generer_timestamp(timestamp, scenario->heure_debut);
        printf("%s %s %s LOGIN %s ERROR\n",
               timestamp, ip, scenario->attaquant,
               FILES[rand() % NB_FILES]);
        usleep(100000); // 0.1 seconde entre les tentatives
    }
    
    // Phase 2: Exploitation
    generer_timestamp(timestamp, scenario->heure_debut + 1);
    printf("%s %s %s LOGIN /etc/passwd SUCCESS\n",
           timestamp, ip, scenario->attaquant);
           
    // Phase 3: Actions malveillantes
    const char* actions_malveillantes[] = {
        "VIEW /etc/passwd", "VIEW /etc/shadow",
        "DOWNLOAD utilisateurs.db", "MODIFY config.xml"
    };
    
    for (int i = 0; i < 4; i++) {
        generer_timestamp(timestamp, scenario->heure_debut + 1);
        printf("%s %s %s %s SUCCESS\n",
               timestamp, ip, scenario->attaquant,
               actions_malveillantes[i]);
        usleep(200000);
    }
    
    // Génération d'alertes
    fprintf(stderr, "ALERT: Possible intrusion detected from %s\n", ip);
    fprintf(stderr, "ALERT: Sensitive files accessed by %s\n", scenario->attaquant);
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    UserState* etats_utilisateurs = calloc(NB_USERS, sizeof(UserState));
    
    // Analyse des arguments
    int mode_realtime = 0;
    int mode_scenario = 0;
    int nb_evenements = 100;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--real-time") == 0) mode_realtime = 1;
        if (strcmp(argv[i], "--scenario") == 0) mode_scenario = 1;
        if (strcmp(argv[i], "--events") == 0 && i+1 < argc) 
            nb_evenements = atoi(argv[i+1]);
    }
    
    // Mode scénario d'attaque
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
    
    // Génération des événements
    for (int i = 0; i < nb_evenements || mode_realtime; i++) {
        generer_evenement(etats_utilisateurs, 1);
        
        if (mode_realtime) {
            usleep(rand() % 1000000); // 0-1 seconde entre les événements
        }
    }
    
    free(etats_utilisateurs);
    return 0;
}