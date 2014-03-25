#include "truc.c"

int main() {
  int **C = NULL;
  List *l = NULL;
  int i,j;
  int nb = 4;
  l = push((void*)1, push((void*)2, push((void*)3, l)));
  
  C = calloc(nb, sizeof(*C));
  failloc(C);
  for(i=0 ; i<nb ; ++i) {
    C[i] = malloc(nb * sizeof(*C[i]));
    failloc(C[i]);
    for(j=0 ; j<nb ; ++j)
      C[i][j] = 2;
  }
  
  print_list(l);

  List* ret = NULL;
 
  ret = f(C,l);
  print_list(ret);
  
  return 0;
}
