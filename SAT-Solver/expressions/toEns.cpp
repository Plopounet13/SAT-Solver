#include <iostream>
#include <set>
#include "expr.hpp"

using namespace std;

vector<set<int>>* toEns(Expr& e){
	vector<set<int>>* v = new vector<set<int>>(1);
	e.toEns(v,1);
	return v;
}


void EImp::toEns(vector<set<int>>* v, int negation){
	return;
}

void EEqiv::toEns(vector<set<int>>* v, int negation){
	return;
}

void EXor::toEns(vector<set<int>>* v, int negation){
	return;
}

void EConst::toEns(vector<set<int>>* v, int negation){
	return;
}

void EVar::toEns(vector<set<int>>* v, int negation){
	v->back().insert(negation * etiq);
}

void ENot::toEns(vector<set<int>>* v, int negation){
    op.toEns(v,-negation);
}

void EOu::toEns(vector<set<int>>* v, int negation){
	op1.toEns(v,negation);
	op2.toEns(v,negation);
}

void EEt::toEns(vector<set<int>>* v, int negation){
	op1.toEns(v,negation);
	v->push_back(set<int>());
	op2.toEns(v,negation);
}
