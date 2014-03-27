#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glpk.h> /* Nous allons utiliser la bibliothèque de fonctions de GLPK */

void go_glpk_go(List *regroup, int len_list, int nb_clients, double capacite_drone) {
	List *it;
	List *it_it;
	#define NBco len_list //nombre de coûts = regroupements
	#define NBcl nb_clients
	#define NBCONTR 1+nb_clients
	#define NBVAR (NBco + NBcl)
	glp_prob *prob; // déclaration d'un pointeur sur le problème
	
	
	/*On crée les "len_list" variables = nombre de regroupements*/
	double x[NBco];
	/*Résultat de la fonction objectif*/
	double z;
	
	
	/* On donne un nom aux variables*/
	char nomvar[NBVAR][3];
	char nomcontr[NBCONTR][12]; /* ici, les contraintes seront nommées "contrainte1", "contrainte2",... */
	
	/* Création d'un problème (initialement vide) */
	prob = glp_create_prob(); /* allocation mémoire pour le problème */
	glp_set_prob_name(prob, "voyageur");
	glp_set_obj_dir(prob, GLP_MIN);
	
	/*On ajoute les contraintes du problème avec ses bornes, i.e. la somme des poids ne doit pas dépasser la capacité du drone, et chaque client est visité une et une seule fois*/
	glp_add_rows(prob, NBCONTR);
	
	glp_set_row_bnds(prob, 1, GLP_UP, 0.0, capacite_drone);
	
	for(i = 2;i <= NBCONTR-1;i++)
	{
		/* partie optionnelle : donner un nom aux contraintes */
		strcpy(nomcontr[i-1], "desservie");
		sprintf(numero[i-1], "%d", i);
		strcat(nomcontr[i-1], numero[i-1]); /* Les contraintes sont nommés "desservie1", "desservie2"... */		
		glp_set_row_name(prob, i, nomcontr[i-1]); /* Affectation du nom à la contrainte i */
		
		/* partie indispensable : les bornes sur les contraintes */
		glp_set_row_bnds(prob, i, GLP_UP, 0.0, 0.0);
	}	
	
	
	/* Déclaration du nombre de variables : len_list */
	
	glp_add_cols(prob, NBco);
	
	/* On précise le type des variables, les indices commencent à 1 également pour les variables! */
	
	for(i = 1;i <= NBco;i++)
	{
		/* partie optionnelle : donner un nom aux variables */
		sprintf(nomvar[i-1],"cout%c",i);
		glp_set_col_name(prob, i , nomvar[i-1]); /* Les variables sont nommées "cout1", "cout2"*/
		
		/* partie obligatoire : bornes éventuelles sur les variables, et type */
		glp_set_col_bnds(prob, i, GLP_DB, 0.0, 1.0);
		glp_set_col_kind(prob, i, GLP_BV);	/*variables binaires, si le regroupement est choisit ou non*/
	}
	
	for(i = 1;i <= NBcl;i++)
	{
		/* partie optionnelle : donner un nom aux variables */
		sprintf(nomvar[i-1],"client%c",'A'+i-1);
		glp_set_col_name(prob, i+NBco , nomvar[i-1]); /* Les variables sont nommées "clientA", "clientB"*/
		
		/* partie obligatoire : bornes éventuelles sur les variables, et type */
		glp_set_col_bnds(prob, i, GLP_DB, 0.0, 1.0); /* bornes sur les variables, comme sur les contraintes */
		glp_set_col_kind(prob, i, GLP_BV);	/*variable binaires, si le client est visité ou non*/
	}

	/* définition des coefficients des variables dans la fonction objectif, tous égaux à 1 ici */

	for(i = 1;i <= NBco;i++) glp_set_obj_coef(prob,i,1);	
	
	/* Optionnel : écriture de la modélisation dans un fichier (TRES utile pour debugger!) */

	glp_write_lp(prob,NULL,"voyageur.lp");

	/* Résolution, puis lecture des résultats */	
	
	glp_simplex(prob,NULL);	glp_intopt(prob,NULL); /* Résolution */
	z = glp_mip_obj_val(prob);
	
	
	//dire a glpk qu'on a len_list variables
	
	for(it=couts ; it ; it=it->tail) {
	
	}
	
	printf("z = %lf\n",z);
	/*for(i = 0;i < NBVAR;i++) printf("x%c = %d, ",'B'+i,(int)(x[i] + 0.5)); /* un cast est ajouté, x[i] pourrait être égal à 0.99999... / 
	puts("");*/
	
	/* libération mémoire */
	glp_delete_prob(prob);
	
	return 0;
}
