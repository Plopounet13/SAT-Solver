#include <iostream>
#include <set>
#include "expr.hpp"

using namespace std;

vector<set<Expr&> >& toEns(Expr& e){
	vector<set<Expr&> >& v = new vector<set<Expr&> >(1);
	e.toEns(v);
	return v;
}


void EImp::toEns(vector<set<Expr&> >& v){
	return;
}

void EEqiv::toEns(vector<set<Expr&> >& v){
	return;
}

void EConst::toEns(vector<set<Expr&> >& v){
	return;
}

void EVar::toEns(vector<set<Expr&> >& v){
	v.back().insert(&self);
}

void ENot::toEns(vector<set<Expr&> >& v){
	v.back().insert(&self);
}

void EOu::toEns(vector<set<Expr&> >& v){
	op1.toEns(v);
	op2.toEns(v);
}

void EEt::toEns(vector<set<Expr&> >& v){
	op1.toEns(v);
	v.push_back(set<Expr&>());
	op2.toEns(v);
}