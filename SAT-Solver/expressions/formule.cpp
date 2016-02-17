#include <iostream>

#include "formule.hpp"

Formule::Formule(Expr& e){
	e.toEns(value);
	activeClauses = new set<int>();
	for (int i=0; i<value.size; ++i){
		activeClauses.insert(i);
	}
}

void Formule::evol(int var){
	for (int c:activeClauses){
		Expr e = Not(Var(var));
		set::set<Expr&>::iterator p = value[c].find(e);
		if(p != set::end){
			activeClauses[c].erase(p);
			if (activeClauses[c].empty()){
				//TODO : backtrack
			}
		}
		e = Var(var);
		p = value[c].find(e);
		if(p != set::end)
			activeClauses.erase(activeClauses.begin()+c);
		if (activeClauses.empty())
			//TODO : Succés
	}
}