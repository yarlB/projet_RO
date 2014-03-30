#include "fonctions.h"

typedef enum {JOHN_TROT, PERSONNAL, NB_ALGOS} Algo_Name;

typedef struct map_algoname_algonum M_Algoname_Algonumber;
struct map_algoname_algonum {
  const int num;
  char const * const name;
} ;

const M_Algoname_Algonumber M_JOHN_TROT = {JOHN_TROT, "JOHN_TROT"} ;

const M_Algoname_Algonumber M_PERSONNAL = {PERSONNAL, "PERSONNAL"} ;

//signatures of fonctions used are the same, there are NB_ALGOS algorithms
typedef List* (*best_cycle_func_t)(int**, List*);
struct {
  best_cycle_func_t algo_bc[NB_ALGOS];
}ALGOS;

