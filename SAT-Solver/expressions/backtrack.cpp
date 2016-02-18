#include "backtrack.hpp"

ElemBacktrack::ElemBacktrack(int vr, bool vl, bool forc, set<int>* clsup, set<int>* clret) : var(vr), val(vl), forced(forc), clauses_sup(clsup), clauses_ret(clret) {}

void ElemBacktrack::annule(vector<set<reference_wrapper<Expr>>>& value, set<int>& activeClauses){
	for (int x: *clauses_sup)
		activeClauses.insert(x);
	
	for (int x: *clauses_ret)
		if (val)
			value[x].insert(*new ENot(*new EVar(var)));
		else
			value[x].insert(*new EVar(var));
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
		if (val){
			Expr* e = new ENot(*new EVar(var));
			set<reference_wrapper<Expr>>::iterator p = value[x].find(*e);
			//e->del(); TODO:
			value[x].erase(p);
			//p->del(); TODO:
		}else{
			Expr* e = new EVar(var);
			set<reference_wrapper<Expr>>::iterator p = value[x].find(*e);
			//e->del(); TODO:
			value[x].erase(p);
			//p->del(); TODO:
		}
	}
	return true;
}

Backtrack::Backtrack(){
	pile = new stack<reference_wrapper<ElemBacktrack>>();
}

void Backtrack::push(int vr, bool vl, bool forc, set<int>* clsup, set<int>* clret){
	ElemBacktrack& e = *new ElemBacktrack(vr,vl,forc,clsup,clret);
	pile->push(e);
}

bool Backtrack::back(vector<set<reference_wrapper<Expr>>>& value, set<int>& activeClauses){
	while (!pile->empty() && pile->top().get().isForced()){
		pile->top().get().annule(value, activeClauses);
		pile->pop();
	}
	if (pile->empty()){
		return false;
	}else{
		return pile->top().get().revert(value, activeClauses);
	}
}


//return 0 if failed
//return 1 if OK