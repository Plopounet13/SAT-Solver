#include "backtrack.hpp"

ElemBacktrack::ElemBacktrack(int vr, bool vl, bool forc, set<int>* clsup, set<int>* clret) : var(vr), val(vl), forced(forc), clauses_sup(clsup), clauses_ret(clret) {}

void ElemBacktrack::annule(vector<set<reference_wrapper<Expr>>>& value, set<int>& activeClauses){
	for (int x: *clauses_sup)
		activeClauses.insert(x);
	
	for (int x: *clauses_ret)
		value[x].insert(*new (val?EVar(var):ENot(EVar(var))));
}

bool ElemBacktrack::isForced(){
	return forced;
}

bool ElemBacktrack::revert(vector<set<reference_wrapper<Expr>>>& value, set<int>& activeClauses){
	
	annule(value,activeClauses);
	
	forced = true;
	val = !val;
	set<int>* tmp = clauses_sup;
	clauses_sup=clauses_ret;
	clauses_ret=tmp;
	
	for (int x: *clauses_sup){
		activeClauses.erase(x);
	}
	
	for (int x: *clauses_ret){
		value[x].erase(val ? ENot(EVar(var)) : EVar(var));
	}
	return true;
}

void Backtrack::push(int vr, bool vl, bool forc, set<int>& clsup, set<int>& clret){
	Elem_backtrack& e = *new Elem_backtrack(vr,vl,forc,clsup,clret);
	pile.push(e);
}

bool Backtrack::back(vector<set<reference_wrapper<Expr>>>& value, set<int>& activeClauses){
	while (!pile.empty() && pile.top().isforced()){
		pile.top().annule(value, activeClauses);
		pile.pop();
	}
	if (pile.empty()){
		return false;
	}else{
		return pile.top().revert(value, activeClauses);
	}
}


//return 0 if failed
//return 1 if OK