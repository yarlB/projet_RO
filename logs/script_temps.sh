#!/bin/bash

palogs="palogs.csv"
jtlogs="jtlogs.csv"

echo "" > $palogs
echo "" > $jtlogs

get () {
    cut -d " " -f $1 <($2 -1 $3)
}

get_head () {
    get $1 head $2
}

get_tail () {
    get $1 tail $2
}

write_csv () {
    for file in $(ls $1);
    do
	datafile=$(echo $file | cut -d "." -f 1)".dat"
	dir=""
	if (echo $datafile | grep -q "A");
	then
	    dir="../A/"
	else
	    dir="../B/"
	fi
	time_before_glpk=$(get_head 6 $file)
	total_time=$(get_tail 3 $file)
	nb_clients=$(head -1 $dir$datafile)
	capacite_drones=$(
	moy=0
	ligne_demande=$(head -3 $dir$datafile | tail -1) 
	for d in $(cut -d " " -f "-"$nb_clients $ligne_demande);
	do
	    moy=$(($moy+$d))
	done
	echo "moy = $moy"
	moy=$(($moy/$nb_clients))
	echo "fichier $datafile , nombre de clients $nb_clients , temps avant glpk : $time_before_glpk , temps total : $total_time, moyenne des demandes $moy"
    done
}

write_csv "*.palog" $palogs

write_csv "*.jtlog" $jtlogs

