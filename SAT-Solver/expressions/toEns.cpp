#include <iostream>
#include "expr.hpp"

using namespace std;

vector<unordered_set<int>>* toEns(Expr& e){
	vector<unordered_set<int>>* v = new vector<unordered_set<int>>(1);
	e.toEns(v,1);
	return v;
}


void EImp::toEns(vector<unordered_set<int>>* v, int negation){
	return;
}

void EEqiv::toEns(vector<unordered_set<int>>* v, int negation){
	return;
}

void EXor::toEns(vector<unordered_set<int>>* v, int negation){
	return;
}

void EConst::toEns(vector<unordered_set<int>>* v, int negation){
	return;
}

void EVar::toEns(vector<unordered_set<int>>* v, int negation){
	v->back().insert(negation * etiq);
}

void ENot::toEns(vector<unordered_set<int>>* v, int negation){
    op.toEns(v,-negation);
}

void EOu::toEns(vector<unordered_set<int>>* v, int negation){
	op1.toEns(v,negation);
	op2.toEns(v,negation);
}

void EEt::toEns(vector<unordered_set<int>>* v, int negation){
	op1.toEns(v,negation);
	v->push_back(unordered_set<int>());
	op2.toEns(v,negation);
}
