#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <assert.h>


enum {
    MAX_PARTICIPANTS = 100,    // Dimensions maximales et longueur des chaînes
    MAX_NOM = 31,             // Un participant : prénom et nom (30 caractères maximum + caractère nul) 
    MAX_CONCOURS = 20,
    MAX_COMMANDE = 31
};


typedef struct {
    char prenom[MAX_NOM];                  // Prénom (30 caractères maximum + caractère nul)
    char nom[MAX_NOM];                    // Nom
    int scores[MAX_CONCOURS];            // scores[i] : score au concours d'indice i (0 si aucun point)
    int rangsPrecedents[MAX_CONCOURS];  // rangsPrecedents[i] : rang lors du dernier BILAN
} Participant;                         // Un participant


typedef struct {
	int nbParticipants;                           // Nombre de participants inscrits
    Participant participants[MAX_PARTICIPANTS];
} Participants;


typedef struct {
    char nom[MAX_NOM];           // Nom du concours (30 caractères maximum + caractère nul)
} Concours;


typedef struct {
    int nbConcours;                       // Nombre de concours créés
    Concours concours[MAX_CONCOURS];
} ListeConcours;


int rangDansConcours(const Participants* p, int indPart, int indConcours);
int nbConcoursDuParticipant(const Participant* part);
int nbParticipantsDuConcours(const Participants* p, int indConcours);


 // Retourne l'indice du participant (prenom, nom) dans p, ou -1 s'il n'est pas encore inscrit. Précondition : p != NULL

int trouverParticipant(const Participants* p, const char prenom[], const char nom[]) {
    assert(p != NULL);
    for (int i = 0; i < p->nbParticipants; ++i)
        if (strcmp(p->participants[i].prenom, prenom) == 0 && strcmp(p->participants[i].nom, nom) == 0)
            return i;
    return -1;
}



/*
  Commande INSCRIRE : lit un prénom et un nom, puis inscrit le participant et affiche "Nom incorrect" s'il est déjà inscrit.
  Précondition : p != NULL et il reste de la place dans le tableau.
*/

void commandeInscrire(Participants* p) {
    assert(p != NULL);
    char prenom[MAX_NOM], nom[MAX_NOM];
    scanf("%s %s", prenom, nom);

    if (trouverParticipant(p, prenom, nom) != -1) {
        printf("Nom incorrect\n");
        return;
    }
    assert(p->nbParticipants < MAX_PARTICIPANTS);
    strcpy(p->participants[p->nbParticipants].prenom, prenom);
    strcpy(p->participants[p->nbParticipants].nom, nom);

    // Le nouveau participant n'a encore aucun point : tous ses scores valent 0

    for (int i = 0; i < MAX_CONCOURS; ++i)
        p->participants[p->nbParticipants].scores[i] = 0;
    ++p->nbParticipants;


    // Son rang actuel dans chaque concours devient son rang précédent

    int indice = p->nbParticipants - 1;
    for (int i = 0; i < MAX_CONCOURS; ++i)
        p->participants[indice].rangsPrecedents[i] = rangDansConcours(p, indice, i);
    printf("Inscription enregistree (%d)\n", p->nbParticipants);
}



/*
  Commande PARTICIPANTS : affiche la liste des participants inscrits, un par ligne, par identifiant croissant.
  Affiche "Aucun participant inscrit" s'il n'y en a aucun.
  (Les scores ne sont pas encore gérés : on affiche 0 concours.). Précondition : p != NULL
*/
void commandeParticipants(const Participants* p) {
    assert(p != NULL);

    if (p->nbParticipants == 0) {
        printf("Aucun participant inscrit\n");
        return;
    }
    for (int i = 0; i < p->nbParticipants; ++i)
        printf("(%d) %s %s : %d concours\n", i + 1, p->participants[i].prenom,  p->participants[i].nom, nbConcoursDuParticipant(&p->participants[i]));
}



/*
Retourne le nombre de participants ayant un score strictement positif dans le concours.
Préconditions : p != NULL et indConcours valide
*/
int nbParticipantsDuConcours(const Participants* p,int indConcours)
{
    int nb = 0;

    for (int i = 0; i < p->nbParticipants; i++)
    {
        if (p->participants[i].scores[indConcours] > 0)
            nb++;
    }

    return nb;
}



//Retourne l'indice du concours de ce nom dans lc, ou -1 s'il n'existe pas. Précondition : lc != NULL
int trouverConcours(const ListeConcours* lc, const char nom[]) {
    assert(lc != NULL);
    for (int i = 0; i < lc->nbConcours; ++i)
        if (strcmp(lc->concours[i].nom, nom) == 0)
            return i;

    return -1;
}


/*
  Commande CREER : lit le nom d'un concours et le crée. Affiche "Nom incorrect" si un concours porte déjà ce nom.
  Précondition : lc != NULL et il reste de la place dans le tableau.
*/
void commandeCreer(ListeConcours* lc) {
    assert(lc != NULL);
    char nom[MAX_NOM];
    scanf("%s", nom);

    if (trouverConcours(lc, nom) != -1) {
        printf("Nom incorrect\n");
        return;
    }
    assert(lc->nbConcours < MAX_CONCOURS);
    strcpy(lc->concours[lc->nbConcours].nom, nom);
    ++lc->nbConcours;
    printf("Creation enregistree (%d)\n", lc->nbConcours);
}



/*
  Commande CONCOURS : affiche la liste des concours créés, un par ligne. Affiche "Aucun concours cree" s'il n'y en a aucun.
  (Les scores ne sont pas encore gérés : on affiche 0 participant.) Précondition : lc != NULL
*/
void commandeConcours(const Participants* p, const ListeConcours* lc) {
    assert(p != NULL);
    assert(lc != NULL);

    if (lc->nbConcours == 0) {
        printf("Aucun concours cree\n");
        return;
    }
    for (int i = 0; i < lc->nbConcours; ++i) {
        int nb = nbParticipantsDuConcours(p, i);
        printf("(%d) %s : %d participant%s\n", i + 1, lc->concours[i].nom, nb, nb > 1 ? "s" : "");
    }
}



//Retourne le nombre de concours pour lesquels le participant a un score non nul. Précondition : part != NULL

int nbConcoursDuParticipant(const Participant* part)
{
    assert(part != NULL);

    int nb = 0;

    for (int i = 0; i < MAX_CONCOURS; ++i)
    {
        if (part->scores[i] > 0)
            ++nb;
    }

    return nb;
}



/*
  Commande GAIN : lit idP, idC et points, puis ajoute les points au score. Vérifications dans l'ordre : participant, concours, points.
  Seule la première erreur est affichée. Préconditions : p != NULL, lc != NULL
*/
void commandeGain(Participants* p, const ListeConcours* lc) {
    assert(p != NULL);
    assert(lc != NULL);
    int idP, idC, points;
    scanf("%d %d %d", &idP, &idC, &points);

    if (idP < 1 || idP > p->nbParticipants) {
        printf("Participant incorrect\n");
        return;
    }
    if (idC < 1 || idC > lc->nbConcours) {
        printf("Concours incorrect\n");
        return;
    }
    if (points <= 0) {
        printf("Points incorrects\n");
        return;
    }
    p->participants[idP - 1].scores[idC - 1] += points;
    printf("Gain enregistre\n");
}




/*
  Commande SCORES : lit idP et affiche les scores non nuls du participant, par identifiant de concours croissant.
  Préconditions : p != NULL, lc != NULL
*/

void commandeScores(const Participants* p, const ListeConcours* lc) {
    assert(p != NULL);
    assert(lc != NULL);
    int idP;
    scanf("%d", &idP);

    if (idP < 1 || idP > p->nbParticipants) {         // Vérification de l'identifiant du participant
        printf("Identifiant incorrect\n");
        return;
    }
	const Participant* part = &p->participants[idP - 1];     // On récupère l'adresse du participant pour simplifier le code
    printf("%s %s\n", part->prenom, part->nom);

    if (nbConcoursDuParticipant(part) == 0) {
        printf("Aucun gain\n");
        return;
    }
    for (int i = 0; i < lc->nbConcours; ++i)
        if (part->scores[i] > 0)
            printf("(%d) %s : %d point%s\n", i + 1, lc->concours[i].nom, part->scores[i], part->scores[i] > 1 ? "s" : "");
}



void afficherPoints(int score) {
    assert(score >= 0);
    printf("%d point", score);
    if (score > 1)
        printf("s");
}


/*
  Retourne le rang du participant d'indice indPart dans le concours d'indice indConcours : 1 + le nombre de participants dont 
  le score est strictement supérieur. Préconditions : p != NULL, indices valides
*/

int rangDansConcours(const Participants* p, int indPart, int indConcours) {
    assert(p != NULL);
    assert(indPart >= 0 && indPart < p->nbParticipants);
    assert(indConcours >= 0 && indConcours < MAX_CONCOURS);
    int rang = 1;
    for (int i = 0; i < p->nbParticipants; ++i)         // On parcourt tous les participants
        if (p->participants[i].scores[indConcours] > p->participants[indPart].scores[indConcours])
            ++rang;
    return rang;
}




/*
  Retourne le nombre de participants qui ont le même score que le participant d'indice indPart au concours d'indice indConcours (lui-même compris).
  Préconditions : p != NULL, indices valides
*/
int nbMemeScore(const Participants* p, int indPart, int indConcours) {
    assert(p != NULL);
    assert(indPart >= 0 && indPart < p->nbParticipants);
    assert(indConcours >= 0 && indConcours < MAX_CONCOURS);
    int nb = 0;
    for (int i = 0; i < p->nbParticipants; ++i)
        if (p->participants[i].scores[indConcours] == p->participants[indPart].scores[indConcours])
            ++nb;
    return nb;
}



/*
  Commande CLASSEMENT : lit idC et affiche le classement du concours.
  Préconditions : p != NULL, lc != NULL
*/
void commandeClassement(const Participants* p, const ListeConcours* lc) {
    assert(p != NULL);
    assert(lc != NULL);
    int idC;
    scanf("%d", &idC);

    if (idC < 1 || idC > lc->nbConcours) {
        printf("Identifiant incorrect\n");
        return;
    }
    int indC = idC - 1;
    printf("%s\n", lc->concours[indC].nom);

    if (nbParticipantsDuConcours(p, indC) == 0) {
        printf("Aucun participant\n");
        return;
    }

    //On affiche d'abord les participants de rang 1, puis de rang 2, etc.
    for (int rang = 1; rang <= p->nbParticipants; ++rang) {
        for (int i = 0; i < p->nbParticipants; ++i) {
            int score = p->participants[i].scores[indC];
            if (score > 0 && rangDansConcours(p, i, indC) == rang) {
                printf("%d", rang);
                if (nbMemeScore(p, i, indC) > 1)
                    printf("ex");
                printf(" -> (%d) %s %s : %d point%s\n", i + 1, p->participants[i].prenom,
                    p->participants[i].nom, score, score > 1 ? "s" : "");
            }
        }
    }
}

/*
  Commande COMPARAISON : lit idC1 et idC2 et compare les deux concours.
  Préconditions : p != NULL, lc != NULL
*/

void commandeComparaison(const Participants* p, const ListeConcours* lc) {
    assert(p != NULL);
    assert(lc != NULL);
    int idC1, idC2;
    scanf("%d %d", &idC1, &idC2);

    // Vérification des identifiants
    if (idC1 < 1 || idC1 > lc->nbConcours || idC2 < 1 || idC2 > lc->nbConcours) {
        printf("Identifiant incorrect\n");
        return;
    }
    int c1 = idC1 - 1;
    int c2 = idC2 - 1;

    // Vérification des participants
    int nbAvecPoints = 0;
    for (int i = 0; i < p->nbParticipants; ++i) {
        int score1 = p->participants[i].scores[c1];
        int score2 = p->participants[i].scores[c2];
        if (score1 > 0 || score2 > 0)
            ++nbAvecPoints;
    }
    if (nbAvecPoints == 0) {
        printf("Aucun participant\n");
        return;
    }

    // Compatibilité des concours
    for (int i = 0; i < p->nbParticipants; ++i) {
        int score1 = p->participants[i].scores[c1];
        int score2 = p->participants[i].scores[c2];
        if (score1 == 0 && score2 > 0) {
            printf("Concours incompatibles\n");
            return;

        }
    }

    // Affichage des résultats
    printf("%s versus %s\n", lc->concours[c2].nom, lc->concours[c1].nom);
    for (int i = 0; i < p->nbParticipants; ++i) {
        int score1 = p->participants[i].scores[c1];
        int score2 = p->participants[i].scores[c2];
        if (score1 > 0) {
            float pourcentage = 100.0 * score2 / score1;
            printf("(%d) %s %s : ", i + 1, p->participants[i].prenom, p->participants[i].nom);
            afficherPoints(score2);
            printf("/");
            afficherPoints(score1);
            printf(" : %.2f%%\n", pourcentage);
        }
    }
}



/*
  Commande BILAN : lit idC, affiche le même classement que CLASSEMENT avec, pour chaque participant, le nombre de places gagnées (+) ou perdues (-)
  depuis le dernier BILAN. Puis mémorise les rangs actuels. Préconditions : p != NULL, lc != NULL
*/
void commandeBilan(Participants* p, const ListeConcours* lc) {
    assert(p != NULL);
    assert(lc != NULL);
    int idC;
    scanf("%d", &idC);

    if (idC < 1 || idC > lc->nbConcours) {
        printf("Identifiant incorrect\n");
        return;
    }
    int indC = idC - 1;
    printf("%s\n", lc->concours[indC].nom);

    if (nbParticipantsDuConcours(p, indC) == 0) {
        printf("Aucun participant\n");
        return;
    }

    // Même affichage que CLASSEMENT : d'abord le rang 1, puis le rang 2, etc.
    for (int rang = 1; rang <= p->nbParticipants; ++rang) {
        for (int i = 0; i < p->nbParticipants; ++i) {
            int score = p->participants[i].scores[indC];
            if (score > 0 && rangDansConcours(p, i, indC) == rang) {
                int variation = p->participants[i].rangsPrecedents[indC] - rang;
                printf("%d", rang);
                if (nbMemeScore(p, i, indC) > 1)
                    printf("ex");
                printf(" -> (%d) %s %s : %d point%s : %+d place%s\n",
                    i + 1, p->participants[i].prenom, p->participants[i].nom,
                    score, score > 1 ? "s" : "",
                    variation, (variation > 1 || variation < -1) ? "s" : "");
            }
        }
    }

    // Les rangs actuels de TOUS les participants deviennent les rangs précédents
    for (int i = 0; i < p->nbParticipants; ++i)
        p->participants[i].rangsPrecedents[indC] = rangDansConcours(p, i, indC);
}

int main() {

    Participants p;
    p.nbParticipants = 0;
    ListeConcours lc;
    lc.nbConcours = 0;
    char cde[MAX_COMMANDE];

    do {                       // Boucle principale : lecture et exécution des commandes
        scanf("%s", cde);
        if (strcmp(cde, "INSCRIRE") == 0)
        {
            commandeInscrire(&p);
        }
        else if (strcmp(cde, "PARTICIPANTS") == 0)
        {
            commandeParticipants(&p);
        }
        else if (strcmp(cde, "CREER") == 0)
        {
            commandeCreer(&lc);
        }
        else if (strcmp(cde, "CONCOURS") == 0)
        {
            commandeConcours(&p, &lc);
        }
        else if (strcmp(cde, "GAIN") == 0)
        {
            commandeGain(&p, &lc);
        }
        else if (strcmp(cde, "SCORES") == 0)
        {
            commandeScores(&p, &lc);
        }
        else if (strcmp(cde, "CLASSEMENT") == 0)
        {
            commandeClassement(&p, &lc);
        }
        else if (strcmp(cde, "COMPARAISON") == 0)
        {
            commandeComparaison(&p, &lc);
        }
        else if (strcmp(cde, "BILAN") == 0)
        {
            commandeBilan(&p, &lc);
        }
    } while (strcmp(cde, "EXIT") != 0);

    return 0;
}

   