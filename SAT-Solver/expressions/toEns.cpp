#include <iostream>
#include <set>
#include "expr.hpp"

using namespace std;

vector<set<reference_wrapper<Expr>>>* toEns(Expr& e){
	vector<set<reference_wrapper<Expr>>>* v = new vector<set<reference_wrapper<Expr>>>(1);
	e.toEns(v);
	return v;
}


void EImp::toEns(vector<set<reference_wrapper<Expr>>>* v){
	return;
}

void EEqiv::toEns(vector<set<reference_wrapper<Expr>>>* v){
	return;
}

void EConst::toEns(vector<set<reference_wrapper<Expr>>>* v){
	return;
}

void EVar::toEns(vector<set<reference_wrapper<Expr>>>* v){
	v->back().insert(*this);
}

void ENot::toEns(vector<set<reference_wrapper<Expr>>>* v){
	v->back().insert(*this);
}

void EOu::toEns(vector<set<reference_wrapper<Expr>>>* v){
	op1.toEns(v);
	op2.toEns(v);
}

void EEt::toEns(vector<set<reference_wrapper<Expr>>>* v){
	op1.toEns(v);
	v->push_back(set<reference_wrapper<Expr>>());
	op2.toEns(v);
}