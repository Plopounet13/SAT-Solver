#!/bin/bash


for heur in "" -rand -moms -dlis
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