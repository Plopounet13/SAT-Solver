#!/bin/bash

if [ $# -lt 1 ]
then
	echo "Nombre de paramêtres incorrect" >&2
	exit
fi


for heur in -dlis "" -moms -rand
do
	if [ "$heur" = "" ]
	then
		echo "Test heuristique standard :"
	else
		echo "Test heuristique $heur :"
	fi
	(time ./resol $* $heur) 2> statTime.tmp
	grep '^real' statTime.tmp | sed s/real//g
	echo
done
	rm -f statTime.tmp
exit 0