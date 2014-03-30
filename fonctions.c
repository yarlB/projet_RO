#include "fonctions.h"

//utilise le tri casier pour fournir une liste de clients(numéros de 1 à nbclients) triée
List* counting_sort(int* demands, int nbclients) {
  int i, min, max;
  List* *basket = NULL;
  List* sorted = NULL;
  if (nbclients < 1)
    return sorted;
  min = max = demands[1];
  for(i=2 ; i<=nbclients ; ++i) {
    if (demands[i]<min) min = demands[i]; //récupération du minimum
    if (demands[i]>max) max = demands[i]; //et du maximum
  }
  basket = calloc(max - min + 1, sizeof(*basket)); //allocation d'un tableau de listes de clients
  failloc(basket);
  for(i=1 ; i<=nbclients ; ++i) //comptage/regroupement
    basket[demands[i]-min] = push((void*)i, basket[demands[i]-min]);
  for(i=max-min ; i>=0 ; --i) { //récupération de la liste triée
    while(basket[i]) {
      sorted = push(basket[i]->head, sorted);

      //Push_Alloc(sorted);
      
      basket[i] = pop(basket[i]);
    }
  }
  free(basket);
  return sorted;
}


/*
  Retourne une liste de listes de numéros de clients : tous les regroupements possibles de clients dont la somme des demande ne dépasse pas capacity.
*/
List* enum_regroups(int* demands, int nbclients, int capacity) {
  List* bases_stack = NULL; //pile des prochaines bases
  Base* base = NULL; //base courante
  Base* tmpbase = NULL; //pour creation
  List* regroups = NULL; //ce que l'on retourne
  int calcul = 0; //pour les calculs temporaires
  List* itclients = NULL; //itérateur sur la liste de clients
  List* tmplist; //pour creation
  
  List* clients = counting_sort(demands, nbclients);
  base = empty_base();
  base->list = push((void*)clients, base->list);
  bases_stack = push((void*)base, bases_stack);
  base = NULL;
  
  while(bases_stack) {
    free(base);
    base = (Base*)bases_stack->head;
    bases_stack = pop(bases_stack);
    itclients = (List*)base->list->head;
    base->list = pop(base->list);
    //tout ça c'était pour récupérer itclients et la base actuelle
    //maintenant on peut itérer sur les clients à partir de itclients pour voir si l'on peut ajouter des nouveaux
    //regroupements et des futures bases, dès que l'on ne peut plus, les clients étant triés, on casse la boucle.
    while(itclients) {
      calcul = base->size + demands[(int)itclients->head];
      if (calcul <= capacity) {
	tmplist = push(itclients->head, base->list);

	
	regroups = push(tmplist, regroups); //on ajoute le nouveau regroupement à la liste de regroupements
	tmplist = push((void*)itclients->tail, tmplist); //on ajoute a la liste l'iterateur vers la liste des clients à
	//partir duquel il faudra regarder quand ce regroupement servira de base (garantit l'unicité de la creation des
	//regroupements)
	tmpbase = empty_base();
	tmpbase->list = tmplist;
	tmpbase->size = calcul;
	bases_stack = push(tmpbase, bases_stack); //on ajoute le regroupement comme future base ici
      } else { 
	break; 
      }
      itclients = itclients->tail;
    }
  }
  free(base);
  return regroups;
}


BC* make_bc(int size) {
  BC* bc = NULL;
  bc = calloc(1, sizeof(*bc));
  failloc(bc);
  bc->t = malloc(sizeof(*(bc->t)) * size);
  failloc(bc->t);
  bc->size = size;
  return bc;
}


inline void swap(Dir_Int *T, int pos1, int pos2) {
  Dir_Int tmp = T[pos1];
  T[pos1] = T[pos2];
  T[pos2] = tmp;
}

int step(Dir_Int *T, int size) {
  int i;
  int mm_i=-1; //max_mobile_index
  int val;
  int pos;
  
  for(i=0 ; i<size ; ++i) {
    pos = T[i].dir + i; //position du voisin
    val = T[i].i; //valeur de l'element
    if (pos < size && pos >= 0 && T[pos].i < val) { //si la position est dans l'intervalle et le voisin est inférieur
                                                       //c'est un mobile
      if(mm_i < 0) //si on a pas deja un mobile ça devient le plus grand mobile
	mm_i = i;
      else if(T[mm_i].i < val) //sinon faut tester
	mm_i = i;
    }
  }
  if(mm_i < 0) //si on a pas trouvé de mobile
    return -1; //point fixe
  
  val = T[mm_i].i; //valeur du mobile
  pos = mm_i + T[mm_i].dir;//position du voisin
  swap(T,mm_i,pos);
  
  for(i=0 ; i<size ; ++i) {
    if(T[i].i > val) 
      T[i].dir = T[i].dir>0 ? -1 : 1; //changement de sens
  }
  return 0; //pas point fixe
}

int cout_f(int **C,Dir_Int *T, int *T_prime, int size) {
  int i;
  int cout = 0;
  for(i=1 ; i<size ; ++i) {
    cout += C[T_prime[T[i].i]][T_prime[T[i-1].i]];
  }
  cout += C[0][T_prime[T[0].i]] + C[T_prime[T[size-1].i]][0];
  return cout;
}

void bc_cpy(Dir_Int *T, int *T_prime, int *perm, int size) {
  int i;
  for(i=0 ; i<size ; ++i) {
    perm[i] = T_prime[T[i].i];
  }
}

//utilisation de l'algorithme Johnson-Trotter pour trouver la meilleure permutation
List* best_cycle(int **C, List *regroup) {
  enum{LEFT = -1, RIGHT = 1};
  int size = len(regroup);
  Dir_Int *T = NULL;
  int *T_prime = NULL;
  int i;
  int cout_tmp;
  List *ret = NULL;
  BC* bc = make_bc(size);


  //allocation de T : le tableau de permutation de couple (i,dir) ou i est un entier entre 0 et size - 1 et dir une 
  //direction, gauche ou droite, -1 ou 1
  T = malloc(size * sizeof(*T));
  failloc(T);
  //allocation de T_prime : le tableau fixe des clients, meme taille et ordre que la liste regroup
  T_prime = malloc(size * sizeof(*T_prime));
  failloc(T_prime);

  //remplissage T_prime
  i=0;
  while(regroup) {
    T_prime[i] = (int)regroup->head;
    regroup = regroup->tail;
    bc->t[i] = T_prime[i];//meilleure permutation au début =)
    ++i;
  }

  //remplissage T
  for (i=0 ; i<size ; ++i) {
    T[i].i = i;
    T[i].dir = LEFT;
  }

#ifdef D_CALL_BEST_CYCLE
  int i_debug;
  printf("best_cycle([");
  for(i_debug=0 ; i_debug<size-1 ; ++i_debug) { 
    printf("%d,", T_prime[i_debug]);
  } 
  printf("%d])\n", T_prime[size - 1]);
  printf("T : "); 
  for(i_debug=0 ; i_debug<size ; ++i_debug) {
    printf("{%d, %s}, ", T[i_debug].i, T[i_debug].dir==LEFT ? "GAUCHE" : "DROITE");
  }
  printf("\n");
  printf("T_prime : ");
  for(i_debug=0 ; i_debug<size ; ++i_debug) {
    printf("%d, ", T_prime[i_debug]); 
  } 
  printf("\n");
#endif
    
  bc->cout = cout_f(C,T,T_prime,size);//cout initial
  while(step(T,size) == 0) {//tant que pas point fixe
    cout_tmp = cout_f(C,T,T_prime,size);
    if (cout_tmp < bc->cout) {
      bc->cout = cout_tmp;
      bc_cpy(T, T_prime, bc->t, size);
#ifdef D_WHILE_BEST_CYCLE
#ifndef D_CALL_BEST_CYCLE
      int i_debug;
#endif
      printf("bc_tmp : ");
      for(i_debug=0 ; i_debug<size ; ++i_debug) {
	printf("%d, ", bc->t[i_debug]);
      }
      printf("\n");
#endif
    }
  }
  
  for(i=0 ; i<bc->size ; ++i) {
    ret = push((void*)bc->t[i], ret);
  }
  free(bc->t);
  ret = push((void*)bc->cout, ret);
  free(bc);
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
#ifdef D_F
    static int loop_nb=0;
    printf("Tour de boucle %d, on sort de la stack :\ncout : %d liste :", ++loop_nb, tmp_cost);
    print_list(tmp);
#endif
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
	    calcul += C[T[(int)tmp->head]][T[i]]; //if the list isn't empty, the head is the head
	  else //the head is 0 (warehouse), then we multiply by 2
	    calcul <<= 1;
#ifdef D_F
	  printf("calcul : %d\n", calcul);
#endif
	  if(calcul < lower_cost) { //interesting
	    tmp2 = copy(tmp);
	    tmp2 = push((void*)i, tmp2);
	    if(len_tmp + 1 == len_regroup) {//very interesting! permutation's complete
	      lower_cost = calcul;
#ifdef D_F
	      printf("lower_cost : %d\n", lower_cost);
#endif
	      free_list(best);
	      best = tmp2;
	    } else {
	      calcul -= C[T[i]][0];
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
  it = best;
  best = NULL;
  while(it) {
    best = push((void*)T[(int)it->head], best);
    it = pop(it);
  }
  return push((void*)lower_cost, best);
}




