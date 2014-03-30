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

void glpk_phase(List *regroups, int nb_clients) {
  List *it, *itit;
  glp_prob *prob; // déclaration d'un pointeur sur le problème
  int *ia;
  int *ja;
  double *ar; // déclaration des 3 tableaux servant à définir la matrice "creuse" des contraintes
  int nbcreux, i, pos, row;
  int len_list = len(regroups);
  
  char nomcontr[nb_clients][20];
  char nomvar[len_list][20]; 
  
  
  /*On crée les "len_list" (nombre de regroupements) variables*/
  double x[len_list +1];
  /*Résultat de la fonction objectif*/
  double z;
  
  
  /* Création d'un problème (initialement vide) */
  prob = glp_create_prob(); /* allocation mémoire pour le problème */
  glp_set_prob_name(prob, "Does androids dream of electric sheep?");
  glp_set_obj_dir(prob, GLP_MIN);
  
  /*On ajoute les contraintes du problème avec ses bornes, i.e. chaque client est visité une et une seule fois*/
#ifdef D_GLPK
  printf("DEBUG_GLPK nombre de contraintes = %d\n", nb_clients);
#endif
  glp_add_rows(prob, nb_clients);
  
  for(i = 1;i <= nb_clients;i++) {
    /* partie optionnelle : donner un nom aux contraintes */
    sprintf(nomcontr[i-1], "client %d livré", i);
    glp_set_row_name(prob, i, nomcontr[i-1]); /* Affectation du nom à la contrainte i */
    
    /* partie indispensable : les bornes sur les contraintes */
    glp_set_row_bnds(prob, i, GLP_FX, 1.0, 0.0);
  }	
  	
  /* Déclaration du nombre de variables : len_list */  
#ifdef D_GLPK
  printf("DEBUG_GLPK nombre de variables = %d\n", len_list);
#endif
  glp_add_cols(prob, len_list);
  
  /* On précise le type des variables, les indices commencent à 1 également pour les variables! */
  
  for(i = 1;i <= len_list;i++) {
    /* partie optionnelle : donner un nom aux variables */
    sprintf(nomvar[i-1],"regroupement %d",i);
    glp_set_col_name(prob, i , nomvar[i-1]); /* Les variables sont nommées"*/
    
    /* partie obligatoire : bornes éventuelles sur les variables, et type */
    glp_set_col_bnds(prob, i, GLP_DB, 0.0, 1.0);
    glp_set_col_kind(prob, i, GLP_BV);	/*variables binaires, si le regroupement est choisit ou non*/
  }
  
  /* définition des coefficients des variables dans la fonction objectif */
  
  for(nbcreux=0, i=1, it=regroups;i < len_list ; ++i, it=it->tail, nbcreux+=len((List*)it->head))
    glp_set_obj_coef(prob, i, (int)((List*)it->head)->head);	
  
  ia = (int *) malloc ((1 + nbcreux) * sizeof(int));
  ja = (int *) malloc ((1 + nbcreux) * sizeof(int));
  ar = (double *) malloc ((1 + nbcreux) * sizeof(double));
  
  for(pos=1, row=0, it=regroups; it ; it=it->tail, ++row) {
    for(itit=((List*)it->head)->tail ; itit ; itit=itit->tail, ++pos) {
      ia[pos] = row;
      ja[pos] = (int)itit->head - 1;
      ar[pos] = 1.0;
    }
  }
  
  glp_load_matrix(prob, nbcreux, ia, ja, ar);
  
  /* Optionnel : écriture de la modélisation dans un fichier (TRES utile pour debugger!) */
  
  glp_write_lp(prob,NULL,"Does_androids_dream_of_electric_sheep.lp");
  
  /* Résolution, puis lecture des résultats */	
  
  glp_simplex(prob,NULL);	
  glp_intopt(prob,NULL); /* Résolution */
  z = glp_mip_obj_val(prob);
 

  printf("z = %lf\n",z);
  /*for(i = 0;i < NBVAR;i++) printf("x%c = %d, ",'B'+i,(int)(x[i] + 0.5)); un cast est ajouté, x[i] pourrait être égal à 0.99999...
    puts("");*/
  
  /* libération mémoire */
  glp_delete_prob(prob);
  free(ia); free(ja); free(ar);
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
  glpk_phase(cycles_costs, p.nblieux);
  
  
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
