#include "backtrack.hpp"
extern int maxVar;

ElemBacktrack::ElemBacktrack(int vr, bool forc, set<int>* clsup, set<int>* clret) : var(vr), forced(forc), clauses_sup(clsup), clauses_ret(clret) {}

//Non appelé par POP ????
ElemBacktrack::~ElemBacktrack(){
cout << "une fois" << endl;
    delete clauses_ret;
    delete clauses_sup;
}

void ElemBacktrack::annule(vector<set<int>>* value, set<int>* activeClauses, map<int,bool>* fixed){
    (*fixed)[var] = false;
	for (int x: *clauses_sup)
		activeClauses->insert(x);
	for (int x: *clauses_ret)
        (*value)[x].insert(-var);
    delete clauses_ret;
    delete clauses_sup;
}

bool ElemBacktrack::isForced(){
	return forced;
}

void ElemBacktrack::revert(vector<set<int>>* value, set<int>* activeClauses, map<int,bool>* fixed, int* vr){
    annule(value,activeClauses,fixed);
	*vr = -var;
}

int ElemBacktrack::variable(){
    if(abs(var)<=maxVar)
        return var;
    else
        return 0;
}

Backtrack::Backtrack(){
	pile = new stack<reference_wrapper<ElemBacktrack>>();
}

void Backtrack::push(int vr, bool forc, set<int>* clsup, set<int>* clret){
	ElemBacktrack& e = *new ElemBacktrack(vr,forc,clsup,clret);
	pile->push(e);
}

bool Backtrack::back(vector<set<int>>* value, set<int>* activeClauses, map<int,bool>* fixed, int* var){
//cout << "BACK"<< endl;
	while (!pile->empty() && pile->top().get().isForced()){
		pile->top().get().annule(value, activeClauses, fixed);
		pile->pop();
	}
	if (pile->empty()){
		return false;
	}else{
		pile->top().get().annule(value, activeClauses, fixed);
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
