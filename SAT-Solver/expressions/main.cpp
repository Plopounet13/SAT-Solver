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

void usage(){
    fprintf(stderr, "Usage : ./resol [-tseitin] src.[cnf|for]\n");
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

    if (argc < 2 || argc > 3){
        fprintf(stderr,"Erreur : nombre de parametres incorrect.\n");
        usage();
        exit(1);
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

    if (!(fd = open(argv[argc-2], O_RDONLY))){
        fprintf(stderr,"Erreur : Ouverture fichier source impossible.\n");
        usage();
        exit(3);
    }

	dup2(fd, 0);

    // parse through the input until there is no more:
    if (argc == 4){
        do {
            yyparse();
            Formule f(res->tseytin());
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
        if (p != 'p' || cnf != "cnf"){
            fprintf(stderr,"Erreur : Le fichier doit commencer par \"p cnf\".\n");
            exit(4);
        }
        vector<set<reference_wrapper<Expr>>>* value = new vector<set<reference_wrapper<Expr>>>();
        while (getline(cin, line)){
            if (line != "" && line[0] != 'c'){
                --C;
                stringstream streamline2(line);
                streamline2 >> var;
                value->push_back(set<reference_wrapper<Expr>>());
                while(var != 0){
                    Expr* e;
                    if (abs(var) > V)
                        fprintf(stderr, "Erreur : Borne max variables dépassée : %d.\n", abs(var));
                    if (var > 0)
                        e = new EVar(var);
                    else
                        e = new ENot(*new EVar(-var));
                    value->back().insert(*e);
                    streamline2 >> var;
                }
            }
        }
        Formule f(value);
        if (C)
            fprintf(stderr, "Erreur : Nombre clauses éronné.\n");
        f.dpll("tash.out");
    }
    return 0;
}

void yyerror(const char *s) {
    cout << "EEK, parse error!  Message: " << s << endl;
    // might as well halt now:
    exit(-1);
}

