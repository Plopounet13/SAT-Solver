#include <cstdio>
#include <cstdlib>
#include <iostream>
using namespace std;

#include "expr.hpp"
#include "formule.hpp"
#include "expr.tab.hpp"

//extern "C" int yyparse();
extern "C" FILE *yyin;
extern Expr *res;
//ici c'est le début du main
int main(int, char**) {
    // parse through the input until there is no more:
    do {
        yyparse();
		Formule f(res->tseytin());
		f.dpll("plop.out");
    } while (!feof(yyin));
    return 0;
}

void yyerror(const char *s) {
    cout << "EEK, parse error!  Message: " << s << endl;
    // might as well halt now:
    exit(-1);
}

