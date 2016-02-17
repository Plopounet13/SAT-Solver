#include <iostream>

#include "formule.hpp"

Formule::Formule(Expr& e){
	e.toEns(value);
	activeClauses = new set<int>();
	for (int i=0; i<value.size; ++i){
		activeClauses.insert(i);
	}
}

int Formule::evol(int var, bool val){
	for (int c:activeClauses){
		Expr e = val?ENot(EVar(var)):EVar(var);
		set<reference_wrapper<Expr> >::iterator p = value[c].find(e);
		if(p != set::end){
            delete(*p);
			value[c].erase(p);
			if (value[c].empty()){

                if(not(b.back())){

                }
				//TODO : backtrack
			}
		}
		e = val?EVar(var):ENot(EVar(var));
		p = value[c].find(e);
		if(p != set::end)
            activeClauses.erase(c);
		if (activeClauses.empty())
			//TODO : Succés
			//Pour tout var positif dans affectation on affiche le machin
	}
}
