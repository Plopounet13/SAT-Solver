#include "backtrack.hpp"
extern int maxVar;

ElemBacktrack::ElemBacktrack(int vr, bool forc, set<int>* clsup, set<int>* clret) : var(vr), forced(forc), clauses_sup(clsup), clauses_ret(clret) {}

//Non appelé par POP ????
ElemBacktrack::~ElemBacktrack(){
cout << "une fois" << endl;
    delete clauses_ret;
    delete clauses_sup;
}

void ElemBacktrack::annule(vector<set<int>>* value, set<int>* activeClauses, map<int,bool>* fixed,vector<int>* nbApparPos,vector<int>* nbApparNeg){
    (*fixed)[var] = false;
	for (int x: *clauses_sup){
		activeClauses->insert(x);
		for(int i:(*value)[x]){
                if(i>0)
                    ++(*nbApparPos)[i];
                else
                    ++(*nbApparNeg)[-i];
        }
    }
	for (int x: *clauses_ret){
        (*value)[x].insert(-var);
        if(-var>0)
            ++(*nbApparPos)[-var];
        else
            ++(*nbApparNeg)[var];
    }
    delete clauses_ret;
    delete clauses_sup;
}

bool ElemBacktrack::isForced(){
	return forced;
}

void ElemBacktrack::revert(vector<set<int>>* value, set<int>* activeClauses, map<int,bool>* fixed, int* vr){
    //annule(value,activeClauses,fixed);
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

bool Backtrack::back(vector<set<int>>* value, set<int>* activeClauses, map<int,bool>* fixed, int* var,vector<int>* nbApparPos,vector<int>* nbApparNeg){
//cout << "BACK"<< endl;
	while (!pile->empty() && pile->top().get().isForced()){
		pile->top().get().annule(value, activeClauses, fixed,nbApparPos,nbApparNeg);
		pile->pop();
	}
	if (pile->empty()){
		return false;
	}else{
		pile->top().get().annule(value, activeClauses, fixed,nbApparPos,nbApparNeg);
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
