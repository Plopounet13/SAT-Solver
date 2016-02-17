#include "backtrack.hpp"

void Elem_backtrack::annule(vector<set<Expr&> >& value, set<int>& activeClauses){
	for (int x:clauses_sup)
		activeClauses.insert(x);
	
	
	
}