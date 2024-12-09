#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Structure pour une unit� lexicale
typedef struct Unite_Lexicale {
    char Lexeme[20];
    int Code;
    int Ligne;
    struct Unite_Lexicale* Suivant;
} UL;

// Fonction pour cr�er une nouvelle unit� lexicale
UL* creerUniteLexicale(const char* lexeme, int code, int ligne) {
    UL* nouvelleUL = (UL*)malloc(sizeof(UL));
    if (nouvelleUL == NULL) {
        printf("Erreur d'allocation m�moire\n");
        exit(EXIT_FAILURE);
    }
    strncpy(nouvelleUL->Lexeme, lexeme, 20);
    nouvelleUL->Code = code;
    nouvelleUL->Ligne = ligne;
    nouvelleUL->Suivant = NULL;
    return nouvelleUL;
}

// Fonction pour ajouter une unit� lexicale � la liste cha�n�e
void ajouterUniteLexicale(UL** liste, UL* nouvelleUL) {
    if (*liste == NULL) {
        *liste = nouvelleUL;
    } else {
        UL* temp = *liste;
        while (temp->Suivant != NULL) {
            temp = temp->Suivant;
        }
        temp->Suivant = nouvelleUL;
    }
}

// Fonction pour afficher la liste des unit�s lexicales
void afficherListeUL(UL* liste) {
    UL* temp = liste;
    printf("\nAfficher la Suite des Unites Lexicales\n");
    while (temp != NULL) {
        printf("Lexeme: [%s]  Code: [%d]  Ligne: [%d]\n", temp->Lexeme, temp->Code, temp->Ligne);
        temp = temp->Suivant;
    }
}

// Lib�ration de m�moire de la liste cha�n�e
void libererListeUL(UL* liste) {
    UL* temp;
    while (liste != NULL) {
        temp = liste;
        liste = liste->Suivant;
        free(temp);
    }
}

// Fonction pour d�terminer le code d'une unit� lexicale
int obtenirCode(const char* lexeme) {
    if(isalpha(lexeme[0])==0) return 260;
    if(isdigit(lexeme[0])==0) return 261;
    if (strcmp(lexeme,"+") == 0 || strcmp(lexeme, "-") == 0 || strcmp(lexeme, "*") == 0 || strcmp(lexeme, "/") == 0) return 262;
    if (strcmp(lexeme, ",") == 0) return 263;
    if (strcmp(lexeme, ";") == 0) return 264;
    if (strcmp(lexeme, "{") == 0) return 265;
    if (strcmp(lexeme, "}") == 0) return 266;
    if (strcmp(lexeme, ">") == 0 || strcmp(lexeme, "<") == 0 || strcmp(lexeme, ">=") == 0 || strcmp(lexeme, "<=") == 0) return 267;
    if (strcmp(lexeme, "or") == 0 || strcmp(lexeme, "and") == 0) return 268;
    if (strcmp(lexeme, "select") == 0) return 269;
    if (strcmp(lexeme, "from") == 0) return 270;
    if (strcmp(lexeme, "where") == 0) return 271;
    if (strcmp(lexeme, "count") == 0) return 272;
    if (strcmp(lexeme, "distinct") == 0) return 273;
    if (strcmp(lexeme, "#") == 0) return 999;
    return 99;  // En cas d'�rreur
}

// Analyse lexicale avec un automate
void Automate(const char* ligne, int numLigne, UL** listeUL) {
    int i = 0, etat = 0;
    char buffer[20] = {0};
    int j = 0;

    while (ligne[i] != '\0') {
        char c = ligne[i];
        switch (etat) {
            case 0: // �tat initial
                if (isspace(c)) {
                    etat = 0;
                } else if(c == '+' || c == '/' || c == '-' || c == '*' || c == '(' || c == ')' ||  c == '=' ||c == ',' || c == ';'){
                    buffer[j++] = c;
                    etat = 0;
                }else if (isalpha(c)) {
                    buffer[j++] = c;
                    etat = 1; // Transition vers �tat identifiant/mot-cl�
                } else if (isdigit(c)) {
                    buffer[j++] = c;
                    etat = 2; // Transition vers �tat nombre
                } else if (c == '.' ) {
                    buffer[j++] = c;
                    etat = 3; // Transition vers �tat nombre .
                } else if (c == '<' || c == '>') {
                    buffer[j++] = c;
                    etat = 5; // Transition vers etat op�rateur relationnel
                } else {
                    printf("Erreur lexicale � la ligne %d, position %d\n", numLigne, i + 1);
                    return;
                }
                break;

            case 1: // Identifiant ou mot-cl�
                if (isalnum(c) || c == '_') {
                    buffer[j++] = c;
                    etat = 1;
                }else if(c == '<' || c == '>') {
                    buffer[j++] = c;
                    etat = 5;
                }else if(c == '+' || c == '/' || c == '-' || c == '*' || c == '(' || c == ')' || c == '=' ||c == ',' || c == ';'){
                    buffer[j++] = c;
                    etat = 0;
                }
                else  {
                    buffer[j] = '\0';
                    ajouterUniteLexicale(listeUL, creerUniteLexicale(buffer, obtenirCode(buffer), numLigne));
                    j = 0;
                    etat = 0; // Retour � l'�tat initial
                    i--; // Reprocesser le caract�re courant
                }
                break;

            case 2:
                if (isdigit(c) ) {
                    buffer[j++] = c;
                    etat = 2 ;
                }else if(c == '.' ) {
                    buffer[j++] = c;
                    etat = 3;
                 }else if(c == '+' || c == '/' || c == '-' || c == '*' || c == '(' || c == ')' || c == ',' || c == '=' || c == ';'){
                    buffer[j++] = c;
                    etat = 0;
                }
                 else {
                    buffer[j] = '\0';
                    ajouterUniteLexicale(listeUL, creerUniteLexicale(buffer, 261, numLigne)); // Code 261 pour un nombre
                    j = 0;
                    etat = 0; // Retour � l'�tat initial
                    i--; // Reprocesser le caract�re courant
                }
                break;

            case 3:
                if (isdigit(c)) {
                    buffer[j++] = c;
                    etat = 4;
                }
                buffer[j] = '\0';
                ajouterUniteLexicale(listeUL, creerUniteLexicale(buffer, 267, numLigne)); // Code 267 pour un op�rateur
                j = 0;
                etat = 0; // Retour � l'�tat initial
                break;

            case 4:
                 if (isdigit(c)) {
                    buffer[j++] = c;
                    etat = 4;
                }
                else if(c == '<' || c == '>') {
                    buffer[j++] = c;
                    etat = 5;
                }
                else if(c == '+' || c == '/' || c == '-' || c == '*' || c == '(' || c == ')' || c == ',' || c == '=' || c == ';'){
                    buffer[j++] = c;
                    etat = 0;
                }
                buffer[j] = '\0';
                ajouterUniteLexicale(listeUL, creerUniteLexicale(buffer, obtenirCode(buffer), numLigne));
                j = 0;
                etat = 0; // Retour � l'�tat initial
                break;
          case 5:
                 if (isdigit(c)) {
                    buffer[j++] = c;
                    etat = 2;
                }
                else if(isalpha(c)) {
                    buffer[j++] = c;
                    etat = 1;
                }
                else if(c == '+' || c == '/' || c == '-' || c == '*' || c == '(' || c == ')' || c == ',' || c == '=' || c == ';'){
                    buffer[j++] = c;
                    etat = 0;
                }
                buffer[j] = '\0';
                ajouterUniteLexicale(listeUL, creerUniteLexicale(buffer, obtenirCode(buffer), numLigne));
                j = 0;
                etat = 0; // Retour � l'�tat initial
                break;
            default:
                printf("Erreur : �tat inconnu\n");
                exit(EXIT_FAILURE);
        }
        i++;
    }

    // Ajouter le dernier lex�me s'il existe
    if (j > 0) {
        buffer[j] = '\0';
        ajouterUniteLexicale(listeUL, creerUniteLexicale(buffer, obtenirCode(buffer), numLigne));
    }
}

// Fonction principale
int main() {
    FILE *fichier = fopen("requete.txt", "r");


    if (fichier == NULL) {
       perror("Erreur : Impossible d'ouvrir le fichier\n");
        return 1;
    }
   /*fprintf(fichier,"select distinct age, salaire, tel\n from emp\n where salaire >= 30000\n");

   fclose(fichier);*/
    UL* listeUL = NULL;
    char ligne[256];
    int numLigne = 1;

    printf("LECTURE ET AFFICHAGE DU CODE SOURCE LIGNE PAR LIGNE\n");

    // Lire le fichier ligne par ligne
    while (fgets(ligne, sizeof(ligne), fichier)) {
        printf("%s", ligne);
        Automate(ligne, numLigne, &listeUL);
        numLigne++;
    }

    fclose(fichier);

    // Ajouter le marqueur final
    ajouterUniteLexicale(&listeUL, creerUniteLexicale("#", obtenirCode("#"), numLigne));

    // Afficher les unit�s lexicales reconnues
    afficherListeUL(listeUL);

    // Lib�rer la m�moire
    // libererListeUL(listeUL);

    return 0;
}