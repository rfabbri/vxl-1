#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef STRUCTURE_DEJA_DECLARE

#define STRUCTURE_DEJA_DECLARE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
/* #include <ieeefp.h> */

#define CODE 1
#define DECODE 2
#define SBSIZE 40
#define CREATE 1
#define INIT 2
#define DPI 6.283185307179

typedef struct struc_Complexe
{
  double real;
  double img;
} Complexe;

typedef struct struc_point
{
  double *valeur, x, y;
  struct struc_point *precedent;
} Point;

typedef struct struc_quadrant
{
  Point *horizontal, *vertical, coin;
  int nbHor, nbVer;
  double theta1, theta2; /* parametres pour le calcul des couts */
} Quadrant;

typedef struct struc_courbe
{
  int ouvert ;
  int nb_points;
  double *theta;
  double *s;
  double *x, *y;
  double *dx, *dy;
} Courbe;

typedef struct struc_matchPoint
{
  double x, y;
} matchPoint;

typedef struct struc_match
{
  int nbPoint;
  matchPoint *racine;
} Match;

typedef struct struc_recalage
{
  double decalage; /* decalage en abscisse curv sur la courbe */
  double angle; /* angle de la rotation a appliquer a la 1ere courbe*/
} Recalage;

typedef struct struc_parprog
{
  Courbe *Crb1; /* premiere courbe */
  Courbe *Crb2; /* deuxieme courbe */
  char nomMatch[200], nFichierQuad[200]; /* nom des fichiers de sauvegardes
                   pour le matching et le paysage */
  double pas; /* pas d'echantillonage pour la prog. dyn. */
  int nbPointEch; /* nombre de points d'echantillonnage pour le recalage
             rapide */
} ParProg;


void matchContoursYounes(char *parameter_file);

void
erreur(char *s);

Courbe *
litcourbe(char *nCrb);
/* prend un nom de fichier, cree une courbe et affecte ses valeurs par
lecture du fichier puis renvoie un pointeur vers la courbe */

Quadrant **
creeQuadrants(Courbe *Crb1, Courbe *Crb2, double pas);

void
miseAJour(Quadrant **quad, int i, int j);

void
nVal(Point *pPoint, Point *qPoint, double theta1, double theta2);  

int
cout(Point *pPoint, Point *qPoint, double theta1, double theta2, 
     double *cVal);

double
retroProp( Quadrant **quad, int iMax, int jMax, Match *Matching);

void
saveMatch(Match *Matching,char *nomMatch, double angle);

void
saveMatch2(Match *Matching, char *nomMatch, double angle, double matchVal);

Complexe *
my_fft(Complexe *A,int N);

Complexe *
complexProd(Complexe *z1, Complexe *z2, int N);

Complexe *
complexPlus(Complexe *z1, Complexe *z2, int N);

Complexe *
complexMoins(Complexe *z1, Complexe *z2, int N);

Recalage *
fastMatch(Courbe *Crb1, Courbe *Crb2);

Courbe *
echCourbe(Courbe *Crb1, int nbPoints);

void
recCourbe(Courbe *Crb1, Courbe *Crb2, Courbe *recCrb1, Courbe *recCbr2, 
Recalage *parRec);

void
freeCourbe(Courbe *Crb);

void
formatCourbe(Courbe *Crb, int nbPoints);

void
sauveQuad(Quadrant **quad, char *nFichierQuad, int l, int c);

void
freeQuad(Quadrant **quad, Courbe *Crb1, Courbe *Crb2); 

void      
progDyn(Quadrant **quad, Courbe *Crb1, Courbe *Crb2);

void
sauveCourbe(Courbe *Crb, char *nFichier, int ouvert);

ParProg *
litParametres(char *fichPar);

void
freeMatch(Match *Matching);






#endif



#ifdef __cplusplus
}
#endif

