#ifndef FONCTIONS_INCL
#define FONCTIONS_INCL
#include "globals.h"

List* counting_sort(int* demands, int nbclients);
List* enum_regroups(int* demands, int nbclients, int capacity);


typedef struct bc BC;
struct bc {
  int cout;
  int *t;
  int size;
};


BC* make_bc(int size);

typedef struct dir_int Dir_Int;
struct dir_int {
  int i;
  char dir; //-1 or 1
};


inline void swap(Dir_Int *T, int pos1, int pos2) {
  Dir_Int tmp = T[pos1];
  T[pos1] = T[pos2];
  T[pos2] = tmp;
}

int step(Dir_Int *T, int size);
int cout_f(int **C,Dir_Int *T, int *T_prime, int size);
void bc_cpy(Dir_Int *T, int *T_prime, int *perm, int size);
List* best_cycle(int **C, List *regroup);
List* f(int **C, List *regroup);

#endif

