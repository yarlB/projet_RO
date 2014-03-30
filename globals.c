#include "globals.h"

//si le pointeur passé en paramètre est NULL, affiche un message d'erreur et exit
//utile pour vérifier la cohérence après une allocation mémoire
void failloc(void* ptr) {
  if(!ptr) {
    fprintf(stderr, "Fail alloc\n");
    exit(1);
  }
}

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

