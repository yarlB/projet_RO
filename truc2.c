#include "fonctions.c"

List* copy_without(List *l, List *w) {
  if(!w)
    return copy(l);
  List *ret = NULL;
  List **itp = &ret;
  while (l) {
    if(l == w)
      l = l->tail;
    *itp = push(l->head, NULL);
    itp = &((*itp)->tail);
  }
  return ret;
}

int best_cost(int **C, List *regroup) {
  List *it, *rest, *last;
  List *stack = NULL;
  int cost, calcul;
  int bc = (int)INFINITY;
  
  stack = push((void*)0, push((void*)0, push((void*)copy(regroup), NULL)));

  while(stack) {
    cost = (int)stack->head;
    stack = pop(stack);
    last = (List*)stact->head;
    stack = pop(stack);
    rest = (List*)stack->head;
    stack = pop(stack);

    if(cost >= bc) {
      free_list(rest);
      continue;
    }

    if(!rest) {
      calcul = cost + C[0][last];
      if (calcul < bc) {
	bc = calcul;
      }
      continue;
    }

    for(it=rest ; it ; it=it->tail) {
      calcul = cost + C[last][(int)it->head];
      if (calcul + C[(int)it->head][0] < bc) {
	stack = push((void*)copy_without(rest,it), stack);
	stack = push((void*)it->head, stack);
	stack = push((void*)calcul, stack);
      }
    }
    free_list(rest);
  }
  return bc;
}
