#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define MAX_JOUEURS 4
#define TAILLE 8

typedef struct {
    // 0 si vide, 1 si blanc, 2 si rouge et 3 si noir
    int pion[TAILLE][TAILLE];
} Plateau;


typedef struct {
    int etat; // 1 si dans la partie, 0 sinon
    int score;
} Joueur;


typedef struct {
    Plateau plateau;
    Joueur joueur[MAX_JOUEURS];
    int nb_joueurs;
    int joueur_courant; // de 1 à 4
    int tour;
    int pion_est_saisi; // 1 si un pion est saisi, 0 sinon
    int pion_i, pion_j; // la ligne et colonne du pion saisi (0 sinon)
} Jeu;

void jeu_ecrire(Jeu *jeu){

    printf("%d %d %d\n",jeu->nb_joueurs,jeu->tour,jeu->joueur_courant);
    for(int i = 0;i<jeu->nb_joueurs;i++){
        printf("%d %d\n",jeu->joueur[i].etat,jeu->joueur[i].score);
    }
    printf("%d %d %d\n", jeu->pion_est_saisi, jeu->pion_i, jeu->pion_j);

    for(int i = 0; i < TAILLE; i++){
        for(int j = 0; j < TAILLE; j++){
            printf("%d ",jeu->plateau.pion[i][j]);
        }
        printf("\n");
    }

}


void jeu_charger(Jeu *jeu) {
    scanf("%d %d %d", &jeu->nb_joueurs, &jeu->tour, &jeu->joueur_courant);

    for (int k = 0; k < jeu->nb_joueurs; k++) {
        scanf("%d %d", &jeu->joueur[k].etat, &jeu->joueur[k].score);    // l'état et le score de chaque joueur
    }

    scanf("%d %d %d", &jeu->pion_est_saisi, &jeu->pion_i, &jeu->pion_j);    // le dernier pion joué/saisi

    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            scanf("%d", &jeu->plateau.pion[i][j]);  // tous les pions du plateau
        }
    }
}


int case_est_valide(int i, int j){
    if (0 > i || i > 7 || 0 > j || j > 7){
        return 0;
    }
    return 1;
}

void jeu_ajoute_score(Jeu *jeu, int valeur) {
    jeu->joueur[jeu->joueur_courant].score += valeur;
}

int jeu_capturer(Jeu *jeu, int i, int j) {
    if (case_est_valide(i,j) > 0) {
        
    int pion = jeu->plateau.pion[i][j];
    if (pion == 0)
        return 0;
    
    if (pion == 1) jeu_ajoute_score(jeu, 1);
    else if (pion == 2) jeu_ajoute_score(jeu, 5);
    else if (pion == 3) jeu_ajoute_score(jeu, 8);     
        jeu->plateau.pion[i][j] = 0;
        return 1;
    } else return 0;
}


int jeu_saisir_pion(Jeu *jeu, int i, int j) {
    if (case_est_valide(i,j) == 0){
        return 0;
    }
    if (jeu->plateau.pion[i][j] != 0) {
        jeu->pion_est_saisi = 1;
        jeu->pion_i = i;
        jeu->pion_j = j;
        return 1;
    } else {
        printf("Impossible de saisir une case sans pion.\n");
        return 0;
    }

}


int jeu_supprime_pion(Jeu *jeu, int i, int j) {
    jeu->plateau.pion[i][j] = 0;
    return 1;
}



int plateau_pion_peut_sauter(Plateau *plateau, int i, int j) {
    if (plateau->pion[i][j] == 0)
        return 0;

    int dirs[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        { 0, -1},          { 0, 1},
        { 1, -1}, { 1, 0}, { 1, 1}
    };

    for (int d = 0; d < 8; d++) {
        int mi = i + dirs[d][0];
        int mj = j + dirs[d][1];
        int di = i + 2 * dirs[d][0];
        int dj = j + 2 * dirs[d][1];

        // Case au milieu doit être un pion, case d'arrivée vide
        if (mi >= 0 && mi < TAILLE && mj >= 0 && mj < TAILLE &&
            di >= 0 && di < TAILLE && dj >= 0 && dj < TAILLE &&
            plateau->pion[mi][mj] != 0 && plateau->pion[di][dj] == 0)
        {
            return 1; // Saut possible
        }
    }
    return 0;
}


int jeu_sauter_vers(Jeu *jeu, int i, int j) {

    // Vérifier qu'un pion est saisi
    if (jeu->pion_est_saisi == 0)
        return 0;

    int si = jeu->pion_i;
    int sj = jeu->pion_j;

    // Case valide ?
    if (i < 0 || i >= TAILLE || j < 0 || j >= TAILLE)
        return 0;

    // Destination doit être vide
    if (jeu->plateau.pion[i][j] != 0)
        return 0;

    int di = i - si;
    int dj = j - sj;

    // Vérifier que le mouvement est un saut de distance 2
    if (!(abs(di) == 2 || abs(dj) == 2))
        return 0;

    // On interdit le cas où |di| + |dj| = 0 (pas de déplacement)
    // ou > 4 (trop loin)
    if (abs(di) + abs(dj) == 0 || abs(di) + abs(dj) > 4)
        return 0;

    // Position de la case sautée
    int mi = si + di / 2;
    int mj = sj + dj / 2;

    // Vérifier que la case intermédiaire est valide et contient un pion
    if (mi < 0 || mi >= TAILLE || mj < 0 || mj >= TAILLE)
        return 0;

    int pion_cible = jeu->plateau.pion[mi][mj];

    if (pion_cible == 0)
        return 0;

    // Ajouter le score selon la valeur du pion sauté
    if (pion_cible == 1)      jeu_ajoute_score(jeu, 1);
    else if (pion_cible == 2) jeu_ajoute_score(jeu, 5);
    else if (pion_cible == 3) jeu_ajoute_score(jeu, 8);

    // Supprimer le pion sauté
    jeu_supprime_pion(jeu, mi, mj);

    // Déplacer le pion saisi
    jeu->plateau.pion[i][j] = jeu->plateau.pion[si][sj];
    jeu->plateau.pion[si][sj] = 0;

    // Mettre à jour la position du pion
    jeu->pion_i = i;
    jeu->pion_j = j;

    // Vérifier si un autre saut est possible
    if (!plateau_pion_peut_sauter(&jeu->plateau, i, j)) {
        jeu->pion_est_saisi = 0; // fin de chaîne de sauts
    }

    return 1;
}

int jeu_arreter(Jeu *jeu){
    int actifs = 0;

    for (int i = 0; i < jeu->nb_joueurs; i++) {
        if (jeu->joueur[i].etat == 1)
            actifs++;
    }
    if (actifs <= 1)
        return 0;
    jeu->joueur[jeu->joueur_courant].etat = 0;

    return 1;
}

int jeu_joueur_suivant(Jeu *jeu){
    int i = 0, index;
    do{
        i++; // On incrémente i jusqu'à ....
        index = (jeu->joueur_courant + i) % 4; // index = 0 ou 1 ou 2 ou 3 (de manière forcée)
    } while (jeu->joueur[index].etat != 1); // On sort de la boucle lorsqu'on arrive sur le joueur_courant

    if((index + 1) == jeu->joueur_courant ) return 0;  // On n'a pas changé de joueur
    if((index+1) < jeu->joueur_courant){
        jeu->tour++;
    }
    jeu->joueur_courant = index;
    jeu->pion_est_saisi = 0;
    jeu->pion_i = 0;
    jeu->pion_j = 0;
    return 1;
}


void swap(int *a, int *b){
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void jeu_table_flip(Jeu *jeu){
/*
Cette fonction consiste à faire une rotation à 180° du plateau de jeu.
*/
    for(int i = 0; i < TAILLE; i++){
        for(int j = 0; j<TAILLE;j++){
            // On ne fait l’échange que pour la moitié des éléments
            if (i < (TAILLE-1-i)|| (i == (TAILLE-1-i) && j < (TAILLE-1-j))){
                swap(&(jeu->plateau.pion[i][j]), &(jeu->plateau.pion[TAILLE-1-i][TAILLE-1-j]));
            }
        }
    }
}

void jeu_afficher(Jeu *jeu){
    
    //-Affichage du score-
    
    printf("Score:\n");
    printf("    ");
    for(int a = 0; a<MAX_JOUEURS;a++){
        printf("J%d  ",a+1);
    }
    printf("\n");
    printf("    ");
    for(int b = 0; b<MAX_JOUEURS;b++){
        printf("%d   ",jeu->joueur[b].score);
    }
    printf("\n");


    printf("Tour: %d\n", jeu->tour);
    printf("Joueur %d    Etat: %d\n",jeu->joueur_courant + 1, jeu->joueur->etat);
    
    // Affichage du plateau -
    printf("    ");
    for(int c = 0; c<TAILLE;c++){
        printf("%d   ",c+1);
    }
    printf("\n  ---------------------------------\n");
    for(int i = 0; i<TAILLE; i++){
        printf("%d |",i+1);
        for(int j = 0; j < TAILLE; j++){
            if(jeu->plateau.pion[i][j]==1){
                if( i == jeu->pion_i && j == jeu->pion_j){
                    printf("[B] ");
                }else{ 
                    printf(" B  ");
                }
            }else if(jeu->plateau.pion[i][j]==2){
                if( i == jeu->pion_i && j == jeu->pion_j){
                    printf("[R] ");
                }else{ 
                    printf(" R  ");
                }
            } else if(jeu->plateau.pion[i][j]==3){
                if( i == jeu->pion_i && j == jeu->pion_j){
                    printf("[N] ");
                }else{ 
                    printf(" N  ");
                }
            }else{
                printf("    ");
            }
        }
        printf("\n");
    }
}

void jeu_prepare_plateau(Jeu *jeu) {
    /* 
    Prépare aléatoirement un plateau de 34 pions blancs,
    20 pions rouges et 10 pions noirs.
    */
    int nb_pions[] = {34, 20, 10}; // Blancs, Rouges, Noirs
    
    for(int i = 0; i < TAILLE; i++) {
        for(int j = 0; j < TAILLE; j++) {
            int couleur;
            do {
                couleur = (rand() % 3) + 1; // Tire 1, 2 ou 3
            } while(nb_pions[couleur - 1] == 0); // Retire si plus de pions de cette couleur
            jeu->plateau.pion[i][j] = couleur;
            nb_pions[couleur - 1]--; // Décrémente
        }
    }
}

int main(void) {
    srand(time(NULL));
    Jeu jeu;
    int action;
    int i, j;

    printf("=== MultiDames — mode test ===\n");
    printf("1. Charger une partie depuis stdin\n");
    printf("2. Initialiser une partie vide (test rapide)\n");
    printf("> ");
    scanf("%d", &action);

    if (action == 1) {
        printf("Entrer les données du jeu selon le format attendu :\n");
        jeu_charger(&jeu);
    } else {
        // exemple minimal
        jeu.nb_joueurs = 2;
        jeu.tour = 1;
        jeu.joueur_courant = 0;
        for (int k = 0; k < 2; k++) {
            jeu.joueur[k].etat = 1;
            jeu.joueur[k].score = 0;
        }
        jeu.pion_est_saisi = 0;
        jeu.pion_i = jeu.pion_j = 0;
        jeu_prepare_plateau(&jeu);
    }

    printf("Partie chargée avec succès !\n");
    jeu_afficher(&jeu);

    while (1) {
        printf("\nActions disponibles :\n");
        printf("1. Capturer un pion\n");
        printf("2. Saisir un pion\n");
        printf("3. Sauter vers une case\n");
        printf("4. Arrêter joueur courant\n");
        printf("5. Joueur suivant\n");
        printf("6. Sauvegarder (jeu_ecrire)\n");
        printf("0. Quitter\n");
        printf("> ");
        scanf("%d", &action);

        if (action == 0) break;

        switch (action) {
            case 1:
                printf("Coordonnées (i j) du pion à capturer : ");
                scanf("%d %d", &i, &j);
                if (!jeu_capturer(&jeu, i, j)) printf("Action impossible.\n");
                break;

            case 2:
                printf("Coordonnées (i j) du pion à saisir : ");
                scanf("%d %d", &i, &j);
                if (!jeu_saisir_pion(&jeu, i, j)) printf("Action impossible.\n");
                break;

            case 3:
                printf("Coordonnées (i j) de destination : ");
                scanf("%d %d", &i, &j);
                if (!jeu_sauter_vers(&jeu, i, j)) printf("Saut impossible.\n");
                break;

            case 4:
                if (!jeu_arreter(&jeu)) printf("Impossible d'arrêter ce joueur.\n");
                break;

            case 5:
                jeu_joueur_suivant(&jeu);
                break;

            case 6:
                printf("\n=== État du jeu (jeu_ecrire) ===\n");
                jeu_ecrire(&jeu);
                printf("===============================\n");
                break;

            default:
                printf("Choix invalide.\n");
                break;
        }

        jeu_afficher(&jeu);
    }

    printf("Fin du test. Merci d’avoir joué !\n");
    return 0;
}
