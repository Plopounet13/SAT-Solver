#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

using namespace std;

#include "expr.hpp"
#include "formule.hpp"
#include "expr.tab.hpp"

//TODO : output debug deductions


//extern "C" int yyparse();
extern "C" FILE *yyin;
extern Expr *res;
extern int maxVar;

void usage(){
    cerr << "Usage : ./resol [help] [-tseitin] src.[cnf|for] [-rand|-moms|-dlis]" << endl;
}

void help(){
	usage();
	cout << endl;
	cout << "Tseitin :" << endl;
	cout << "\tFichier source .for" << endl;
	cout << "\tCompatible heuristique" << endl;
	cout << endl;
	cout << "CNF :" << endl;
	cout << "\tFichier source .cnf" << endl;
	cout << "\tCompatible heuristique" << endl;
	cout << endl;
	cout << "Heuristique :" << endl;
	cout << "\t-rand : Choix aléatoire de la variable à fixer." << endl;
	cout << "\t-moms : Choix de la variable ayant le plus d'occurences dans les clauses de taille minimale" << endl;
	cout << "\t-dlis : Choix de la variable satisfaisant le plus de clauses" << endl;
	cout << endl;
	cout << "Exemples :" << endl;
	cout << "\t./resol -tseitin fic.for -rand" << endl;
	cout << "\tCet appel applique tseitin à fic.for puis fait tourner dpll en pariant aléatoirement" << endl;
	cout << "\t./resol fic.cnf" << endl;
	cout << "\tCet appel fait tourner dpll en pariant la première variable libre trouvée" << endl;
	exit(0);
}

char* getExt(char* param){
    int i;
    for(i=0; param[i]; ++i);
    while (i > 0 && param[--i]!='.');
    if (param[i]=='.')
        return param+i+1;
    else
        return NULL;
}

//ici c'est le début du main
int main(int argc, char** argv) {
	if (!strcmp(argv[1],"help"))
		help();
    if (argc < 2 || argc > 4){
        fprintf(stderr,"Erreur : nombre de parametres incorrect.\n");
        usage();
        exit(1);
    }
	int heuristique=STANDARD;
	if (argc == 4){
		if (!strcmp(argv[3], "-rand"))
			heuristique=RAND;
		else if (!strcmp(argv[3], "-moms"))
			heuristique=MOMS;
		else if (!strcmp(argv[3], "-dlis"))
			heuristique=DLIS;
		--argc;
	}

    if (argc == 3 && strcmp(argv[1], "-tseitin")){
        fprintf(stderr,"Erreur : parametre inconnu : %s.\n", argv[1]);
        usage();
        exit(2);
    }

    char* ext;
    if (!((argc == 3 && (ext = getExt(argv[2])) && !strcmp(ext, "for")) || (argc == 2 && (ext = getExt(argv[1])) && !strcmp(ext, "cnf")))){
        fprintf(stderr,"Erreur : extension invalide.\n");
        usage();
        exit(3);
    }
    int fd, dstd;

    if (!(fd = open(argv[argc-1], O_RDONLY))){
        fprintf(stderr,"Erreur : Ouverture fichier source impossible.\n");
        usage();
        exit(3);
    }
	dup2(fd, 0);
    // parse through the input until there is no more:
    if (argc == 3){
        do {
            yyparse();
            Formule f(res->tseytin(maxVar), heuristique);
            f.dpll("plop.out");
        } while (!feof(yyin));
    } else {
        int var, V, C;
        char p;
        string line, cnf;
        getline(cin, line);
        while (line[0] == 'c'){
            getline(cin, line);
        }
        stringstream streamline(line);
        streamline >> p >> cnf >> V >> C;
        maxVar = V;
        if (p != 'p' || cnf != "cnf"){
            fprintf(stderr,"Erreur : Le fichier doit commencer par \"p cnf\".\n");
            exit(4);
        }
        vector<set<int>>* value = new vector<set<int>>();
        while (getline(cin, line)){
            if (line != "" && line[0] != 'c' and line != "0" and line != "%"){
                --C;
                stringstream streamline2(line);
                streamline2 >> var;
                value->push_back(set<int>());
                while(var != 0){
                    Expr* e;
                    if (abs(var) > V){
                        fprintf(stderr, "Erreur : Borne max variables dépassée : %d.\n", abs(var));
                        maxVar = abs(var);
                    }
                    value->back().insert(var);
                    streamline2 >> var;
                }
            }
        }
        Formule f(value, heuristique);
        if (C){
            fprintf(stderr, "Erreur : Nombre clauses éronné.\n");
        }
        f.dpll("tash.out");
    }
    return 0;
}

void yyerror(const char *s) {
    cout << "EEK, parse error!  Message: " << s << endl;
    // might as well halt now:
    exit(-1);
}

