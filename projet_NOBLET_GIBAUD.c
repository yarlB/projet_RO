 /* NOM1 Prénom1
    NOM2 Prénom2 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glpk.h> /* Nous allons utiliser la bibliothèque de fonctions de GLPK */

#include <time.h>
#include <sys/time.h>
#include <sys/resource.h> /* Bibliothèques utilisées pour mesurer le temps CPU */

#include "strategy.c" //Notre bibliothèque


/* structures et fonctions de mesure du temps CPU */

struct timeval start_utime, stop_utime;

void crono_start() {
  struct rusage rusage;
	
  getrusage(RUSAGE_SELF, &rusage);
  start_utime = rusage.ru_utime;
}

void crono_stop() {
  struct rusage rusage;
  
  getrusage(RUSAGE_SELF, &rusage);
  stop_utime = rusage.ru_utime;
}

double crono_ms() {
  return (stop_utime.tv_sec - start_utime.tv_sec) * 1000 +
    (stop_utime.tv_usec - start_utime.tv_usec) / 1000 ;
}

/* Structure contenant les données du problème */

typedef struct {
  int nblieux; /* Nombre de lieux (incluant le dépôt) */
  int capacite; /* Capacité du véhicule de livraison */
  int *demande; /* Demande de chaque lieu (la case 0 est inutilisée car le dépôt n'a aucune demande à voir satisfaire) */
  int **C; /* distancier (les lignes et colonnes 0 correspondent au dépôt) */
} donnees;

/* lecture des donnees */

void lecture_data(char *file, donnees *p) {
  int i,j;
  FILE *fin;
  
  int val;
  fin = fopen(file,"rt");
  
  /* Lecture du nombre de villes */
  
  fscanf(fin,"%d",&val);
  p->nblieux = val;
  
  /* Allocation mémoire pour la demande de chaque ville, et le distancier */
  
  p->demande = (int *) malloc (val * sizeof(int));
  p->C = (int **) malloc (val * sizeof(int *));
  for(i = 0;i < val;i++) p->C[i] = (int *) malloc (val * sizeof(int));
  
  /* Lecture de la capacité */
  
  fscanf(fin,"%d",&val);
  p->capacite = val;
  
  /* Lecture des demandes des clients */
  
  for(i = 1;i < p->nblieux;i++) {
    fscanf(fin,"%d",&val);
    p->demande[i] = val;
  }
  
  /* Lecture du distancier */
  
  for(i = 0; i < p->nblieux; i++)
    for(j = 0; j < p->nblieux; j++) {
      fscanf(fin,"%d",&val);
      p->C[i][j] = val;
    }
  
  fclose(fin);
}

/* Fonction de libération mémoire des données */

void free_data(donnees *p) {
  int i;
  for(i = 0;i < p->nblieux;i++) free(p->C[i]);
  free(p->C);
  free(p->demande);	
}


/* Fonction d'initialisation de la structure de données anonyme ALGOS permettant de choisir entre l'algorithme 
   johnson-trotter ou notre algorithme personnel pour trouver le meilleur cycle */

void init_strategies() {
  ALGOS.algo_bc[JOHN_TROT] = &best_cycle;
  ALGOS.algo_bc[PERSONNAL] = &f;
}

int main(int argc, char *argv[]) {	
  /* Déclarations des variables (à compléter) */
  
  donnees p; 
  double temps;
  List* regroups = NULL; //c'est la qu'on stocke la liste des regroupements (qui sont des listes)
  List* it = NULL;
  List *cycles_costs = NULL;
  List *ret;

  Algo_Name algo_to_use;
  
  if (argc<3) {
    printf("2 paramètres sont attendus : 1) nom du fichier de données, 2) nom de l'algorithme pour la résolution de la phase 2\nJOHN_TROT pour l'algorithme johnson-trotter ou PERSONNAL pour notre algorithme.\n");
    exit(1);
  }

  if(strcmp(argv[2], M_JOHN_TROT.name) == 0) {
    algo_to_use = M_JOHN_TROT.num;
  } else if(strcmp(argv[2], M_PERSONNAL.name) == 0) {
    algo_to_use = M_PERSONNAL.num;
  } else {
    printf("Les algos disponibles sont : JOHN_TROT et PERSONNAL\n");
    exit(1);
  }
  
  /* initialisation de la structure des strategys possibles */
  init_strategies();
  
  /* Chargement des données à partir d'un fichier */
  
  lecture_data(argv[1],&p);
  
  /* Lancement de la résolution... */
  
  crono_start(); // .. et donc du chronomètre
  
  //Récupération de l'ensemble(liste) des regroupements possibles
  //phase1
  regroups = enum_regroups(p.demande, p.nblieux - 1, p.capacite);


#ifdef D_LONGUEUR_REGROUPS
  printf("longueur regroups : %d\n", len(regroups));
#endif

#ifdef D_AFFICHAGE_REGROUPS
  it = regroups;
  printf("DEBUG\nregroups :\n");
  while(it) {
    print_list(((List*)it->head));
    printf("\n");
    it = it->tail;
  }
  printf("ENDEBUG\n");
#endif

  //phase2
  //Résolution de chaque instance du voyageur de commerce pour chaque regroupement

  it = regroups;
  while(it) {
    ret = ALGOS.algo_bc[algo_to_use](p.C, ((List*)it->head));
#ifdef D_COUT_CYCLE
    printf("cout : %d\n", (int)ret->head);
#endif
    cycles_costs = push((void*)ret, cycles_costs);
    pop(it->head);
    it = pop(it);
  }

#ifdef D_PARCOURS_COUT
  List* it2;
  List *l_debug;
  it = cycles_costs;
  printf("DEBUG\n[Cout] : parcours\n"); 
  while(it) {
    l_debug = (List*)it->head;
    printf("[%d] : 0, ", (int)l_debug->head);
    for(it2=l_debug->tail ; it2 ; it2=it2->tail) {
      printf("%d, ", (int)it2->head);
    }
    printf("0\n");
    it = it->tail;
  }
  printf("ENDEBUG\n");
#endif


  //programmation et résolution de l'instance glpk
  
  

  
  //vidange mémoire
  while(cycles_costs) {
    free_list((List*)cycles_costs->head);
    cycles_costs = pop(cycles_costs);
  }
  /* Problème résolu, arrêt du chrono */
  
  crono_stop();
  temps = crono_ms()/1000,0;
  
  /* Affichage des résultats (à compléter) OUI OUI!! */
  
  printf("Temps : %f\n",temps);	
  
  /* libération mémoire (à compléter en fonction des allocations) */
  
  free_data(&p);
  
  /* J'adore qu'un plan se déroule sans accroc! */
  return 0;
}
