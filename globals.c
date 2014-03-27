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
#endif



void failloc(void* ptr) {
  if(!ptr) {
    fprintf(stderr, "Fail alloc\n");
    exit(1);
  }
}

//assez parlant
typedef struct list List;
struct list {
  void *head;
  List *tail;
};

//ajout d'un élément en tête de liste
List* push(void* head, List *tail) {
  List *list = NULL;
  list = malloc(sizeof(*list));
  failloc(list);
  list->head = head;
  list->tail = tail;
  return list;
}

//destruction du premier élément
List* pop(List *list) {
  List *prec = list;
  list = list->tail;
  free(prec);
  return list;
}

//destruction d'une liste
void free_list(List *list) {
  while(list) {
    list = pop(list);
  }
}

//un ensemble de demandes est une liste... on conserve la somme des demandes des clients de la liste
typedef struct base Base;
struct base {
  int size;
  List* list;
};

Base* empty_base() {
  Base* base = NULL;
  base = malloc(sizeof(*base));
  failloc(base);
  base->size = 0;
  base->list = NULL;
  return base;
}


void print_list(List* l) {
  while (l) {
    printf("%d, ", (int)l->head);
    l = l->tail;
  }
  printf("\n");
}

//copie renversante!
List* rev_copy(List* l) {
  List* ret = NULL;
  while(l) {
    ret = push(l->head, ret);
    l = l->tail;
  }
  return ret;
}

int len(List* l) {
  int acc = 0;
  while(l) {
    ++acc;
    l = l->tail;
  }
  return acc;
}



List* copy(List *l) {
  List *ret = NULL;
  List **itt = &ret;
  while(l) {
    *itt = push(l->head, NULL);
    itt = &((*itt)->tail);
    l = l->tail;
  }
  return ret;
}

