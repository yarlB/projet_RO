#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glpk.h> /* Nous allons utiliser la bibliothèque de fonctions de GLPK */

void go_glpk_go(List *couts, int len_list, int nb_clients) {
	List *it;
	
	glp_prob *prob;
	double x[len_list];
	
	//create_prob
	prob = glp_create_prob();
	glp_set_prob_name(prob, "voyageur");
	glp_set_obj_dir(prob, GLP_MIN);
	
	//dire a glpk qu'on a len_list variables
	
	for(it=couts ; it ; it = it->tail) {
	
	}
}
