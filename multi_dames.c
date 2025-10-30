#include <stdio.h>
#include <stdlib.h>


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

    FILE *fichier = fopen("fichier_sauvegarde","w");

    fprintf(fichier,"%d %d %d\n",jeu->nb_joueurs,jeu->tour,jeu->joueur_courant);
    for(int i = 0;i<MAX_JOUEURS;i++){
        fprintf(fichier,"%d %d\n",jeu->joueur[i].etat,jeu->joueur[i].score);
    }
    fprintf(fichier, "%d %d %d\n", jeu->pion_est_saisi, jeu->pion_i, jeu->pion_j);

    for(int i = 0; i < TAILLE; i++){
        for(int j = 0; j < TAILLE; j++){
            fprintf(fichier,"%d ",jeu->plateau.pion[i][j]);
        }
        fprintf(fichier,"\n");
    }

    fclose(fichier);
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


int jeu_capturer(Jeu *jeu, int i, int j) {
    if (case_est_valide(i,j) > 0) {
        jeu->plateau.pion[i][j] = 0;
        return 1;
    } else return 0;
}


int jeu_saisir_pion(Jeu *jeu, int i, int j) {
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


int jeu_supprime_pion(Jeu* jeu, int depart_i, int depart_j, int arrivee_i, int arrivee_j) {
    if (depart_i == arrivee_i) {
        jeu->plateau.pion[depart_i][arrivee_j-depart_j] = 0;
    } else if (depart_j == arrivee_j) {
        jeu->plateau.pion[arrivee_i-depart_i][depart_j] = 0;
    } else {
        jeu->plateau.pion[arrivee_i-depart_i][arrivee_j-depart_j] = 0;
    }
    return 1;
}


int jeu_ajoute_score(Jeu* jeu, int depart_i, int depart_j, int arrivee_i, int arrivee_j) {
    if (depart_i == arrivee_i) {
        jeu->joueur[jeu->joueur_courant].score += jeu->plateau.pion[depart_i][arrivee_j-depart_j];
        return 1;
    } else if (depart_j == arrivee_j) {
        jeu->joueur[jeu->joueur_courant].score += jeu->plateau.pion[arrivee_i-depart_i][depart_j];
        return 1;
    } else {
        jeu->joueur[jeu->joueur_courant].score += jeu->plateau.pion[arrivee_i-depart_i][arrivee_j-depart_j];
        return 1;
    }
}


int jeu_sauter_vers(Jeu *jeu, int i, int j) {
    int case_depart_i = jeu->pion_i;
    int case_depart_j = jeu->pion_j;
    if (jeu->plateau.pion[i][j] == 0) {
        if ((abs(jeu->pion_i-i)<=2 && abs(jeu->pion_j-j)<=2) && (abs(jeu->pion_i-i)!=0 && abs(jeu->pion_j-j)!=0)) {
            jeu_ajoute_score(jeu, case_depart_i, case_depart_j, i, j);
            jeu_supprime_pion(jeu, case_depart_i, case_depart_j, i, j);
            jeu->plateau.pion[i][j] = jeu->plateau.pion[jeu->pion_i][jeu->pion_j];  // on remplace la case d'arrivee par la couleur du pion de depart
            jeu->plateau.pion[jeu->pion_i][jeu->pion_j] = 0;    // on met à NULL le pion de départ

            return 1;
        }
    }
    return 0;
}

int jeu_arreter(Jeu *jeu){
    if(jeu->nb_joueurs == 1) return 0; //S'il reste un joueur, on continue la partie.
    jeu->joueur[jeu->joueur_courant - 1].etat--;
    jeu->nb_joueurs--;
    return 1;
}

int jeu_joueur_suivant(Jeu *jeu){
    int i = 0, index;
    do{
        i++; // On incrémente i jusqu'à ....
        index = (jeu->joueur_courant + i) % 4; // index = 0 ou 1 ou 2 ou 3 (de manière forcée)
    } while (jeu->joueur[index].etat != 1); // On sort de la boucle lorsqu'on arrive sur le joueur_courant

    if((index + 1) == jeu->joueur_courant ) return 0;  // On n'a pas changé de joueur
    jeu->joueur_courant = index;
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
    printf("  ");
    for(int a = 0; a<TAILLE;a++){
        printf(" %d",a+1);
    }
    printf("\n  ----------------\n");
    for(int i = 0; i<TAILLE; i++){
        printf("%d |",i+1);
        for(int j = 0; j < TAILLE; j++){
            if(jeu->plateau.pion[i][j]==1){
                printf("B ");
            }else if(jeu->plateau.pion[i][j]==2){
                printf("R ");
            } else if(jeu->plateau.pion[i][j]==3){
                printf("N ");
            }else{
                printf("  ");
            }
        }
        printf("\n");
    }
}


int main(void) {
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
        for (int x = 0; x < TAILLE; x++)
            for (int y = 0; y < TAILLE; y++)
                jeu.plateau.pion[x][y] = (x + y) % 4 == 0 ? 1 : 0;
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
