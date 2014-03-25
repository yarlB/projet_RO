#include "fonctions.c"

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


//What we keep are indexes of spots in an array : 'T'. Thus, 'best' keeps the best cycle (of indexes) found so far, 
//the 'stack' keeps lists of indexes etc.
//we make bunches of calls to malloc, thus, this can be '"managing memory" time' consuming, an idea to not be such a brat
//to your beloved kernel would be to allocate big chunks of memory then distribute them when asked.
//malloc(BIG * sizeof(List)) here we got an array!
List*
f(int **C, List *regroup) {
  int lower_cost = (int)INFINITY; //best cost found yet
  int tmp_cost, calcul, len_tmp; //some temporary variables
  int len_regroup = len(regroup); 
  int i;
  List *best = NULL; 
  List *tmp = NULL, *it, *tmp2;
  List *stack = NULL;
  int *T = NULL;
  char *mark = NULL;
  T = malloc(sizeof(*T) * len_regroup);
  failloc(T);
  mark = calloc(len_regroup, sizeof(*mark));
  failloc(T);

  for(i=0, it=regroup ; it ; it=it->tail, ++i)
    T[i] = (int)it->head;

#ifdef D_F
  int i_debug;
  printf("DEBUG F([");
  for(i_debug=0 ; i_debug<len_regroup ; ++i_debug) 
    printf("%d, ", T[i_debug]);
  printf("])\n");
#endif

  //first set in stack is empty set with a cost of 0
  stack = push((void*)push((void*)0,NULL), stack);

  //while some elements remains untreated
  while (stack) {
    tmp = (List*)stack->head;
    stack = pop(stack);
    tmp_cost = (int)tmp->head;
    tmp = pop(tmp);
    //heuristic : even if the path of spots isn't complete, if the cost of the path + the cost to go from the last spot
    //to the spot zero exceed the lower cost we found so far, then we can not keep going on this branch
    calcul = tmp_cost + (tmp ? C[T[(int)tmp->head]][0] : 0); //if tmp's empty then cost to go from 0 to 0 is...
    if(calcul >= lower_cost) {
      free_list(tmp);
      continue;
    } else {
      //we maintain 'len_tmp' to count elements 
      //and set 'mark' to know what elements are in the list
      len_tmp = 0;
      for(it=tmp ; it ; it=it->tail, ++len_tmp) {
	mark[(int)it->head] = 1;
      }
      
      for(i=0 ; i<len_regroup ; ++i) {
	if(mark[i]) //is already element
	  continue;
	else {
	  calcul = C[T[i]][0] + tmp_cost; //we had the lowest 'going home' cost
	  if(tmp)
	    calcul += C[T[(int)tmp->head]][i]; //if the list isn't empty, the head is the head
	  else //the head is 0 (warehouse), then we multiply by 2
	    calcul <<= 1;
	  
	  if(calcul < lower_cost) { //interesting
	    tmp2 = copy(tmp);
	    tmp2 = push((void*)i, tmp2);
	    ++len_tmp;
	    if(len_tmp - 1 == len_regroup) {//very interesting!
	      lower_cost = calcul;
	      free_list(best);
	      best = tmp2;
	    } else {
	      calcul -= C[T[(int)tmp2->head]][0];
	      stack = push((void*)push((void*)calcul, tmp2), stack);
	    }
	  }
	}
      }
      free_list(tmp);
    }
    for(i=0 ; i<len_regroup ; ++i)
      mark[i] = 0;
  }
  return push((void*)lower_cost, best);
}
