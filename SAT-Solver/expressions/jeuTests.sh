#!/bin/bash

if [ ! -f listeTests.txt ]
then
	echo "Le fichier listeTests.txt n'existe pas veuillez le créer" >&2
	echo "(Un fichier à tester par ligne)" >&2
	exit 1
fi

RED=`tput setaf 1`
GREEN=`tput setaf 2`
NC=`tput sgr0`

while read test
do
	line=($test)
	fichier=${line[0]}
	repAtt=${line[1]}
	if [[ "$fichier" == *.cnf ]]; then
		(time ./resol $fichier $*) 2> jeuTests.tmp > resolAnswer.tmp
	elif [[ "$fichier" == *.for ]]; then
		(time ./resol -tseitin $fichier $*) 2> jeuTests.tmp > resolAnswer.tmp
	else
		echo "${RED}Extension de \"$fichier\" incorrecte${NC}" >&2
		echo
		continue
	fi
	echo "$fichier $*:"
	grep '^real' jeuTests.tmp | sed s/real//g
	repDon=`grep '^s ' resolAnswer.tmp | sed s/'s '//g`
	if [ $repDon == $repAtt ]
	then
		echo "$repDon [${GREEN}OK${NC}]"
	else
		echo "$repDon [${RED}KO${NC}]"
	fi
	echo
done <listeTests.txt
rm -f jeuTests.tmp resolAnswer.tmp