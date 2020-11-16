#include <stdio.h>
#include <stdlib.h>

#define TAILLEMAX_L 12  // taille Max du labyrinthe avec murs externes

// -----------------------------------------------------------------------------
// Structures de donnees
// -----------------------------------------------------------------------------

unsigned int CodageLabyrinthe[TAILLEMAX_L][TAILLEMAX_L]={
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,1,1,1,1,0,1,1,1,1,0},
  {0,1,1,1,1,1,0,1,1,0,1,0},
  {0,1,0,0,0,1,0,1,1,0,1,0},
  {0,1,1,1,0,1,0,1,1,1,1,0},
  {0,1,1,1,0,1,1,1,1,0,1,0},
  {0,1,1,1,0,1,1,1,1,0,1,0},
  {0,1,1,1,1,1,1,1,1,1,1,0},
  {0,1,1,0,1,1,1,1,1,0,1,0},
  {0,1,0,0,0,0,0,0,0,0,1,0},
  {0,1,1,1,1,1,1,1,1,1,1,0},
  {0,0,0,0,0,0,0,0,0,0,0,0}
};

// On a donc en termes de coordonnees :
// (0,0),(0,1),(0,2),...,(0,TAILLEMAX_L-1)
// (1,0),(1,1),(1,2),...,(1, TAILLEMAX_L-1)
// ...
// (TAILLEMAX_L-1,0),...,(TAILLEMAX_L-1,TAILLEMAX_L-1)
// avec
// Entree du labyrinthe : [1][TAILLEMAX_L-2]
// Sortie du labyrinthe : [TAILLEMAX_L-2][1]

typedef struct coordonnees {
  unsigned int lin;
  unsigned int col;
} COORDONNEES;

COORDONNEES * Entree, * Sortie;

typedef struct chemin {
  COORDONNEES * Coordonnees;
  struct chemin * CrossedCells;
} CHEMIN;

CHEMIN * TrajectoireRobot;


// -----------------------------------------------------------------------------
// Acces a la cellule suivante
// -----------------------------------------------------------------------------

COORDONNEES * NextCellNord(COORDONNEES * CoordonneesCourantes) {
  // Coordonnees de la case nord a partir de la case courante
  static COORDONNEES * CoordonneesNext;

  CoordonneesNext = malloc(sizeof(COORDONNEES));
  CoordonneesNext->lin=CoordonneesCourantes->lin-1;
  CoordonneesNext->col=CoordonneesCourantes->col;
  return CoordonneesNext;
}

COORDONNEES * NextCellEst(COORDONNEES * CoordonneesCourantes) {
  // Coordonnees de la case est a partir de la case courante
  static COORDONNEES * CoordonneesNext;

  CoordonneesNext = malloc(sizeof(COORDONNEES));
  CoordonneesNext->lin=CoordonneesCourantes->lin;
  CoordonneesNext->col=CoordonneesCourantes->col+1;
  return CoordonneesNext;
}

COORDONNEES * NextCellSud(COORDONNEES * CoordonneesCourantes) {
  // Coordonnees de la case sud a partir de la case courante
  static COORDONNEES * CoordonneesNext;

  CoordonneesNext = malloc(sizeof(COORDONNEES));
  CoordonneesNext->lin=CoordonneesCourantes->lin+1;
  CoordonneesNext->col=CoordonneesCourantes->col;
  return CoordonneesNext;
}

COORDONNEES * NextCellOuest(COORDONNEES * CoordonneesCourantes) {
  // Coordonnees de la case ouest a partir de la case courante
  static COORDONNEES * CoordonneesNext;

  CoordonneesNext = malloc(sizeof(COORDONNEES));
  CoordonneesNext->lin=CoordonneesCourantes->lin;
  CoordonneesNext->col=CoordonneesCourantes->col-1;
  return CoordonneesNext;
}


// -----------------------------------------------------------------------------
// Initialisations
// -----------------------------------------------------------------------------

COORDONNEES * InitCoordonnees(unsigned int lin, unsigned int col) {
  // Affectation de coordonnees
  static COORDONNEES * UneCase;

  UneCase      = malloc(sizeof(COORDONNEES));
  UneCase->lin = lin;
  UneCase->col = col;
  return UneCase;
}


// -----------------------------------------------------------------------------
// Gestion des listes de pointeurs (Chemin et Frontiere)
// -----------------------------------------------------------------------------

CHEMIN * AjouteDansChemin(COORDONNEES * Coordonnees, CHEMIN * CheminConnu) {
  // Ajoute une cellule du labyrinthe au chemin connu, et renvoie le nouveau
  // chemin obtenu
  static CHEMIN * Cell;

  Cell               = malloc(sizeof(CHEMIN));
  Cell->Coordonnees  = Coordonnees;
  Cell->CrossedCells = CheminConnu;
  
  return Cell;
}

void EffaceDansChemin(CHEMIN * DebutChemin) {
  // Rend la mémoire precedemment allouée a partir de DebutChemin
  CHEMIN * CellSuivante;

  CellSuivante = DebutChemin->CrossedCells;
  while (CellSuivante != NULL) {
    free(DebutChemin);
    DebutChemin = CellSuivante;
    CellSuivante = DebutChemin->CrossedCells;
  }
}


// -----------------------------------------------------------------------------
// Recherche et Affichage
// -----------------------------------------------------------------------------

void AfficherCodageLabyrinthe(void) {
  // Affiche murs et passages du labyrinthe tel que code dans CodageLabyrinthe[][]
  unsigned int lin, col;

  for (lin=0; (lin<TAILLEMAX_L); ++lin) {
    for (col=0; (col<TAILLEMAX_L); ++ col) {
      printf("%c",!CodageLabyrinthe[lin][col]?'*':' ');
      printf(" ");
    }
    printf("\n");
  }
}

void AfficherParcours (CHEMIN * Cell) {
  // Affiche la cellule parcourue
  printf("\n lin = %d ",Cell->Coordonnees->lin); printf("col = %d, ",Cell->Coordonnees->col);
}

unsigned int Egal(COORDONNEES * Cell1, COORDONNEES * Cell2) {
  // Verifie si Cell1 est egale a Cell2
  return (Cell1->lin==Cell2->lin) && (Cell1->col==Cell2->col);
}

unsigned int EnDehors(COORDONNEES * Cell, CHEMIN * CheminParcouru) {
  // Verifie l'appartenance de Cell a CheminParcouru
  
  while ((CheminParcouru != NULL) && (!Egal(Cell,CheminParcouru->Coordonnees)))
    CheminParcouru=CheminParcouru->CrossedCells;
    return (CheminParcouru == NULL);
}

unsigned int Ouvert (COORDONNEES * CellCourante) {
  // Renvoie le statut de la cellule courante : mur ou passage
  return CodageLabyrinthe[CellCourante->lin][CellCourante->col];
}

CHEMIN * Recherche(COORDONNEES * Entree, COORDONNEES * Sortie) {
  // Renvoi du premier chemin de l'Entree vers la Sortie dans le Labyrinthe

  COORDONNEES * CellSuivante;
  static CHEMIN * CheminCourant;

  // Initialisation du chemin
  CheminCourant = AjouteDansChemin(Entree,NULL);
  
  while (!Egal(CheminCourant->Coordonnees, Sortie)) {
      CellSuivante   = NextCellNord(CheminCourant->Coordonnees);
      if (Ouvert(CellSuivante) && EnDehors(CellSuivante,CheminCourant)) {
        CheminCourant = AjouteDansChemin(CellSuivante, CheminCourant);
        AfficherParcours (CheminCourant);
        printf("nord\n");
      }
      else {
        CellSuivante = NextCellEst(CheminCourant->Coordonnees);
        if (Ouvert(CellSuivante) && EnDehors(CellSuivante,CheminCourant)) {
          CheminCourant = AjouteDansChemin(CellSuivante, CheminCourant);
          AfficherParcours (CheminCourant);
          printf("est\n");
        }
  else {
          CellSuivante = NextCellSud(CheminCourant->Coordonnees);
          if (Ouvert(CellSuivante) && EnDehors(CellSuivante,CheminCourant)) {
            CheminCourant = AjouteDansChemin(CellSuivante, CheminCourant);
            AfficherParcours (CheminCourant);
            printf("sud\n");
          }
    else {
            CellSuivante = NextCellOuest(CheminCourant->Coordonnees);
            if (Ouvert(CellSuivante) && EnDehors(CellSuivante,CheminCourant)) {
              CheminCourant = AjouteDansChemin(CellSuivante, CheminCourant);
              AfficherParcours (CheminCourant);
              printf("ouest\n");
            }
    }
  }
   }
  }
  return CheminCourant;
}


// -----------------------------------------------------------------------------
// main
// -----------------------------------------------------------------------------

int main (void) {
  Entree = InitCoordonnees(1,TAILLEMAX_L-2);
  Sortie = InitCoordonnees(TAILLEMAX_L-2,1);

  AfficherCodageLabyrinthe();
  TrajectoireRobot = Recherche(Entree, Sortie);

  return 0;
}