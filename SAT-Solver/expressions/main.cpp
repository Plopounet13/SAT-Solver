#include <cstdio>
#include <cstdlib>
#include <iostream>
using namespace std;

#include "expr.hpp"
#include "expr.tab.hpp"

//extern "C" int yyparse();
extern "C" FILE *yyin;
extern Expr *res;
//ici c'est le début du main
int main(int, char**) {
//on déclare un map qui sert de fonction d'évaluation
//0 : faux
//1 : vrai
//2 : non assigné
    map<int,int> sigma;
    // parse through the input until there is no more:
    do {
        yyparse();
        cout << res->to_string() << endl << res->eval(sigma) << endl;
    } while (!feof(yyin));
    return 0;
}

void yyerror(const char *s) {
    cout << "EEK, parse error!  Message: " << s << endl;
    // might as well halt now:
    exit(-1);
}

