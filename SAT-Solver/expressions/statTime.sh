#!/bin/bash

for heur in -rand -moms -dlis
do
	echo "Test heuristique $heur :"
	(time ./resol $* $heur) 2>&1|grep '^real'|sed s/real//g
	echo
done

echo "Test heuristique standard :"
(time ./resol $* $heur) 2>&1|grep '^real'|sed s/real//g

exit 0