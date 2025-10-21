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
    int joueur_courant;
    int tour;
    int pion_est_saisi; // 1 si un pion est saisi, 0 sinon
    int pion_i, pion_j; // la ligne et colonne du pion saisi (0 sinon)
} Jeu;


int jeu_capturer(Jeu *jeu, int i, int j) {
    if (0<=i && i<=7 && 0<=j && j<=7) {
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
    } else if (depart_j == arrivee_j) {
        jeu->joueur[jeu->joueur_courant].score += jeu->plateau.pion[arrivee_i-depart_i][depart_j];
    } else {
        jeu->joueur[jeu->joueur_courant].score += jeu->plateau.pion[arrivee_i-depart_i][arrivee_j-depart_j];
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
