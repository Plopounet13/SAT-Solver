#include "backtrack.hpp"

ElemBacktrack::ElemBacktrack(int vr, bool vl, bool forc, set<int>* clsup, set<int>* clret) : var(vr), val(vl), forced(forc), clauses_sup(clsup), clauses_ret(clret) {}

void ElemBacktrack::annule(vector<set<int>>* value, set<int>* activeClauses, map<int,int>* fixed){
    (*fixed)[var] = 0;
	for (int x: *clauses_sup)
		activeClauses->insert(x);

	for (int x: *clauses_ret)
		if (val)
			(*value)[x].insert(-var);
		else
			(*value)[x].insert(var);
}

bool ElemBacktrack::isForced(){
	return forced;
}

void ElemBacktrack::revert(vector<set<int>>* value, set<int>* activeClauses, map<int,int>* fixed, int* vr){
    annule(value,activeClauses,fixed);
	*vr = var;
}

int ElemBacktrack::variable(){
    if(var>0)
        return val?var:(-var);
    else
        return 0;
}

Backtrack::Backtrack(){
	pile = new stack<reference_wrapper<ElemBacktrack>>();
}

void Backtrack::push(int vr, bool vl, bool forc, set<int>* clsup, set<int>* clret){
	ElemBacktrack& e = *new ElemBacktrack(vr,forc,clsup,clret);
	pile->push(e);
}

bool Backtrack::back(vector<set<int>>* value, set<int>* activeClauses, map<int,int>* fixed, int* var){
	while (!pile->empty() && pile->top().get().isForced()){
		pile->top().get().annule(value, activeClauses, fixed);
		//TODO: del
		pile->pop();
	}
	if (pile->empty()){
		return false;
	}else{
		pile->top().get().revert(value, activeClauses, fixed, var);
		//TODO: del
		pile->pop();
		return true;
	}
}

void Backtrack::variables(set<int>& s){
    int v;
    while (!pile->empty()){
        v=pile->top().get().variable();
        if(v!=0)
            s.insert(v);
		//TODO: del
		pile->pop();
	}
}
