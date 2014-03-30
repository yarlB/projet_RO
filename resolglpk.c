void go_glpk_go(List *regroups, int nb_clients) {
  List *it, *itit;
  glp_prob *prob; // déclaration d'un pointeur sur le problème
  int *ia;
  int *ja;
  double *ar; // déclaration des 3 tableaux servant à définir la matrice "creuse" des contraintes
  int nbcreux, i, pos, row;
  int len_list = len(regroups);
  
  char nomcontr[nb_clients +1][20];
  char nomvar[len_list +1][20]; 
  
  
  /*On crée les "len_list" (nombre de regroupements) variables*/
  double x[len_list +1];
  /*Résultat de la fonction objectif*/
  double z;
  
         
  /* Création d'un problème (initialement vide) */
  prob = glp_create_prob(); /* allocation mémoire pour le problème */
  glp_set_prob_name(prob, "Does androids dream of electric sheep?");
  glp_set_obj_dir(prob, GLP_MIN);
	
  /*On ajoute les contraintes du problème avec ses bornes, i.e. chaque client est visité une et une seule fois*/
  glp_add_rows(prob, nb_clients);
  
  for(i = 2;i <= nb_clients;i++) {
    /* partie optionnelle : donner un nom aux contraintes */
    sprintf(nomcontr[i-1], "client %d livré", i);
    glp_set_row_name(prob, i, nomcontr[i-1]); /* Affectation du nom à la contrainte i */
    
    /* partie indispensable : les bornes sur les contraintes */
    glp_set_row_bnds(prob, i, GLP_FX, 1.0, 0.0);
  }	
  
	
  /* Déclaration du nombre de variables : len_list */  
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
  
  for(nbcreux=0, i=1, it=regroups;i <= len_list;++i, it=it->tail, nbcreux+=len((List*)it->head))
    glp_set_obj_coef(prob, i, (int)((List*)it->head)->head);	
  
  ia = (int *) malloc ((1 + nbcreux) * sizeof(int));
  ja = (int *) malloc ((1 + nbcreux) * sizeof(int));
  ar = (double *) malloc ((1 + nbcreux) * sizeof(double));
  
  for(pos=1, row=0, it=regroups; it ; it=it->tail, ++row) {
    for(itit=(List*)it->head ; itit ; itit=itit->tail) {
      ia[pos] = row;
      ja[pos] = (int)itit->head;
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
}
