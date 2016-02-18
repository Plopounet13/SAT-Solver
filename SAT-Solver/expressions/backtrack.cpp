#include "backtrack.hpp"

ElemBacktrack::ElemBacktrack(int vr, bool vl, bool forc, set<int>* clsup, set<int>* clret) : var(vr), val(vl), forced(forc), clauses_sup(clsup), clauses_ret(clret) {}

void ElemBacktrack::annule(vector<set<reference_wrapper<Expr>>>* value, set<int>* activeClauses){
	for (int x: *clauses_sup)
		activeClauses->insert(x);
	
	for (int x: *clauses_ret)
		if (val)
			(*value)[x].insert(*new ENot(*new EVar(var)));
		else
			(*value)[x].insert(*new EVar(var));
}

bool ElemBacktrack::isForced(){
	return forced;
}

void ElemBacktrack::revert(vector<set<reference_wrapper<Expr>>>* value, set<int>* activeClauses, int* vr, bool* b){
	
	annule(value,activeClauses);
	*vr = var;
	*b = !val;
}

Backtrack::Backtrack(){
	pile = new stack<reference_wrapper<ElemBacktrack>>();
}

void Backtrack::push(int vr, bool vl, bool forc, set<int>* clsup, set<int>* clret){
	ElemBacktrack& e = *new ElemBacktrack(vr,vl,forc,clsup,clret);
	pile->push(e);
}

bool Backtrack::back(vector<set<reference_wrapper<Expr>>>* value, set<int>* activeClauses, int* var, bool* b){
	while (!pile->empty() && pile->top().get().isForced()){
		pile->top().get().annule(value, activeClauses);
		//TODO: del
		pile->pop();
	}
	if (pile->empty()){
		return false;
	}else{
		pile->top().get().revert(value, activeClauses, var, b);
		//TODO: del
		pile->pop();
		return true;
	}
}


//return 0 if failed
//return 1 if OK