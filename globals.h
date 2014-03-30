#ifndef INCL_GLOBALS
#define INCL_GLOBALS

#include <stdlib.h> //exit
#include <stdio.h> //fprintf
#include <string.h> //malloc
#include <math.h> //INFINITY

#ifdef DEBUG
#define D_LONGUEUR_REGROUPS
#define D_AFFICHAGE_REGROUPS
#define D_COUT_CYCLE
#define D_PARCOURS_COUT
#define D_CALL_BEST_CYCLE
#define D_WHILE_BEST_CYCLE
#define D_F
#define D_GLPK
#endif

void failloc(void* ptr);

//assez parlant
typedef struct list List;
struct list {
  void *head;
  List *tail;
};

List* push(void* head, List *tail);
List* pop(List *list);
void free_list(List *list);

//un ensemble de demandes est une liste... on conserve la somme des demandes des clients de la liste
typedef struct base Base;
struct base {
  int size;
  List* list;
};

Base* empty_base();
void print_list(List* l);
List* rev_copy(List* l);
int len(List* l);
List* copy(List *l);

#endif
