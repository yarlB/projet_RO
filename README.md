projet_RO
=========
compilation simple johnson trotter algorithme:
gcc -Wall -D RESOL_JOHN_TROT -o projet_RO_JT projet_NOBLET_GIBAUD.c

compilation simple algorithme maison:
gcc -Wall -o projet_RO_AM projet_NOBLET_GIBAUD.c

compilation avec debug :
-choisir le/les debugs dans globals.c, exemple pour tous les debugs:
gcc -Wall -o projet_RO_DEBUG -D DEBUG projet_NOBLET_GIBAUD.c
