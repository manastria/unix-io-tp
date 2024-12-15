# Unix I/O et Analyse de Logs - Ressources pédagogiques

Ce dépôt contient un ensemble de programmes en C conçus pour enseigner et explorer les concepts de redirection d'entrées/sorties sous Unix ainsi que l'analyse de logs système. Ces outils permettent une progression pédagogique des concepts de base jusqu'à des scénarios d'analyse de sécurité réalistes.

## Contenu du dépôt

### Programmes de base pour comprendre les flux standard
- **base_io.c** : Programme d'introduction aux entrées/sorties standard (stdin, stdout, stderr)
- **compteur.c** : Compte et numérote les lignes reçues sur l'entrée standard
- **fluxstandard.c** : Démontre les différents flux standard et leur redirection
- **numeroteur.c** : Version avancée du compteur avec des options supplémentaires
- **prefixeur.c** et **prefixeur2.c** : Ajoutent des préfixes aux lignes, utiles pour comprendre le traitement de texte en flux

### Programme d'analyse de logs
- **loggen.c** : Générateur de logs système sophistiqué avec plusieurs modes de fonctionnement :
  - Génération chronologique sur 24h
  - Surveillance en temps réel
  - Simulation de scénarios d'attaque

## Installation

```bash
git clone https://github.com/votrecompte/unix-io-logs.git
cd unix-io-logs
make
```

Le Makefile compilera tous les programmes nécessaires. Assurez-vous d'avoir gcc installé sur votre système.

## Utilisation pédagogique

Ces programmes sont conçus pour une progression pédagogique en trois phases :

1. **Compréhension des redirections Unix**
   ```bash
   ./base_io > sortie.txt
   ./base_io 2> erreurs.txt
   ./base_io > sortie.txt 2> erreurs.txt
   ```

2. **Manipulation des pipes**
   ```bash
   ./base_io | ./compteur
   ./base_io | ./prefixeur
   ```

3. **Analyse de sécurité**
   ```bash
   ./loggen --real-time
   ./loggen --scenario
   ```

## Exemples d'utilisation avancée

### Surveillance de sécurité en temps réel
```bash
./loggen --real-time | grep --line-buffered -E "DELETE|MODIFY" | \
  awk '{
    if ($4 == "DELETE") print "\033[31m[ALERTE] Suppression :" $0 "\033[0m";
    else print "\033[33m[ATTENTION] Modification :" $0 "\033[0m";
  }'
```

### Analyse des tentatives d'intrusion
```bash
./loggen | grep "^\[2[0-3]\|^\[0[0-4]:" | grep "ERROR\|DENIED"
```

## Structure du projet
```
.
├── src/
│   ├── base_io.c
│   ├── compteur.c
│   ├── fluxstandard.c
│   ├── loggen.c
│   ├── numeroteur.c
│   ├── prefixeur.c
│   └── prefixeur2.c
├── Makefile
└── README.md
```

## Objectifs pédagogiques

- Comprendre les concepts de redirection sous Unix (>, >>, 2>, |)
- Maîtriser la manipulation des flux standard en C
- Apprendre l'analyse de logs système
- S'initier à la détection d'intrusion basique
- Pratiquer les commandes Unix de traitement de texte (grep, awk, sort)

## Contribution

Les contributions sont les bienvenues ! N'hésitez pas à proposer des améliorations via des pull requests ou à signaler des problèmes dans les issues.

## Licence

Ce projet est sous licence MIT. Voir le fichier [LICENSE](./LICENSE) pour plus de détails.

---
Développé pour l'enseignement des systèmes Unix et de la sécurité informatique.
