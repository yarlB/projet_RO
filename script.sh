#!/bin/bash

ls_with_dir () {
    sed -e "s:.*:$1&:" <(ls $1) 
}

for file in $( ls_with_dir "A/" ; ls_with_dir "B/" )
do
    basefile=$1$(basename $file)
    ( exec1="./projet $file PERSONNAL > $basefile.palog" ;
    echo $exec1 ; 
    eval $exec1 ) &
    ( exec2="./projet $file JOHN_TROT > $basefile.jtlog" ;
    echo $exec2 ;
    eval $exec2 )
done
