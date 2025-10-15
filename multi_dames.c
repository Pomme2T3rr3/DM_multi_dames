#include <stdio.h>


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

}
