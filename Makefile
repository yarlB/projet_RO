all: 
	gcc -o projet --std=c99 -Wall -Wextra -pedantic projet_NOBLET_GIBAUD.c fonctions.c globals.c -lglpk
