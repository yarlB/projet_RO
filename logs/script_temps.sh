#!/bin/bash

palogs="palogs.resume"
jtlogs="jtlogs.resume"

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
	capacite_drones=$(head -2 $dir$datafile | tail -1)
	moy=0
	ligne_demande=$(head -3 $dir$datafile | tail -1) 
	for d in $(echo $ligne_demande | cut -d " " -f "-"$nb_clients)
	do
	    moy=$(($moy+$d))
	done
	moy=$(($moy/$nb_clients))
	echo "fichier $datafile :"
	echo -e "-nombre de clients $nb_clients\n-temps avant glpk : $time_before_glpk\n-temps total : $total_time\n-moyenne des demandes $moy\n-capacite drones : $capacite_drones\n"
    done
} >> $2

write_csv "*.palog" $palogs

write_csv "*.jtlog" $jtlogs

