#include "backtrack.hpp"
extern int maxVar;
extern bool bcl;

ElemBacktrack::ElemBacktrack(int vr, bool forc, set<int>* clsup, set<int>* clret) : var(vr), forced(forc), clauses_sup(clsup), clauses_ret(clret) {}

ElemBacktrack::~ElemBacktrack(){
	delete clauses_sup;
}

void ElemBacktrack::annule(vector<set<int>>* value, set<int>* activeClauses, map<int,int>* fixed,vector<int>* nbApparPos,vector<int>* nbApparNeg, int &lastBack){
    (*fixed)[var] = 0;
	for (int x: *clauses_sup){
		activeClauses->insert(x);
		if(!bcl){
            for(int i:(*value)[x]){
                    if(i>0)
                        ++((*nbApparPos)[i]);
                    else
                        ++((*nbApparNeg)[-i]);
            }
		}
    }
	for (int x: *clauses_ret){
        (*value)[x].insert(-var);
        if(!bcl){
            if(-var>0)
                ++(*nbApparPos)[-var];
            else
                ++(*nbApparNeg)[var];
        }
    }
	lastBack=var;
}

bool ElemBacktrack::isForced(){
	return forced;
}

void ElemBacktrack::revert(vector<set<int>>* value, set<int>* activeClauses, map<int,int>* fixed, int* vr){
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
	pile = new stack<ElemBacktrack>();
	lastBack = 0;
}

void Backtrack::push(int vr, bool forc, set<int>* clsup, set<int>* clret){
	pile->emplace(vr,forc,clsup,clret);
}

bool Backtrack::back(vector<set<int>>* value, set<int>* activeClauses, map<int,int>* fixed, int* var,vector<int>* nbApparPos,vector<int>* nbApparNeg){
//cout << "BACK"<< endl;
	while (!pile->empty() && pile->top().isForced()){
		pile->top().annule(value, activeClauses, fixed,nbApparPos,nbApparNeg, lastBack);
		pile->pop();
	}
	if (pile->empty()){
//cout << "ENDBACK UNSAT" << endl;
		return false;
	}else{
		pile->top().annule(value, activeClauses, fixed,nbApparPos,nbApparNeg, lastBack);
		pile->pop();
//cout << "ENDBACK" << endl;
		return true;
	}
}

void Backtrack::variables(set<int>& s){
    int v;
    while (!pile->empty()){
        v=pile->top().variable();
        if(v!=0)
            s.insert(v);
		//TODO: del
		pile->pop();
	}
}
