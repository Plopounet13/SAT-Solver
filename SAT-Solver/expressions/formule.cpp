#include <iostream>

#include "formule.hpp"

Formule::Formule(Expr& e){
	value = new vector<set<reference_wrapper<Expr>>>();
	e.toEns(value);
	activeClauses = new set<int>();
	for (int i=0; i<value->size(); ++i){
		activeClauses->insert(i);
	}
}
//0:continue, 1:succeed, 2:fail
int Formule::evol(int var, bool val, bool forced){
	set<int>* clauses_sup = new set<int>();
	set<int>* clauses_ret = new set<int>();
	set<pair<int,bool> > forcedVariables;
	for (int c:*activeClauses){
		Expr* e;
		if (val)
			e = new ENot(*new EVar(var));
		else
			e = new EVar(var);
		set<reference_wrapper<Expr> >::iterator p = (*value)[c].find(*e);
		if(p != (*value)[c].end()){
            // TODO : p->get().del();
            clauses_ret->insert(c);
			(*value)[c].erase(p);
			if ((*value)[c].empty()){
                b.push(var,val,forced,clauses_sup,clauses_ret);
                if(b.back(value, activeClauses, &var, &val)){
                    return evol(var, val, true);
                }
                else
                    return 2;
			}
			if ((*value)[c].size()==1){
                if (EVar* expVar = dynamic_cast<EVar*>(&((*value)[c].begin())->get()))
                    forcedVariables.insert(pair<int,bool>(expVar->getEtiq(),true));
				else if (ENot* expNot = dynamic_cast<ENot*>(&((*value)[c].begin())->get())){
					EVar* varlala = dynamic_cast<EVar*>(&expNot->getOp());
                    forcedVariables.insert(pair<int,bool>(varlala->getEtiq(),false));
				}
			}
		}
		if (val)
			e = new EVar(var);
		else
			e = new ENot(*new EVar(var));
		p = (*value)[c].find(*e);
		if(p != (*value)[c].end()){
            activeClauses->erase(c);
            clauses_sup->insert(c);
        }
		if (activeClauses->empty()){
            b.push(var,val,forced,clauses_sup,clauses_ret);
            return 1;
		}
	}
	b.push(var,val,forced,clauses_sup,clauses_ret);
    for(auto& x:forcedVariables){
        int r = evol(get<0>(x),get<1>(x),true);
        if(r!=0)
            return r;
    }
	return 0;
}

pair<int,bool> Formule::choose() {
    Expr& e = *((*value)[*(activeClauses->begin())].begin());
	if(ENot* expnot = dynamic_cast<ENot*>(&e)){
		EVar* varlala = dynamic_cast<EVar*>(&expnot->getOp());
        return pair<int,bool>(varlala->getEtiq(),false);
	}
	else{
		EVar* expvar = dynamic_cast<EVar*>(&e);
        return pair<int,bool>(expvar->getEtiq(),true);
	}
}
//TODO : écrire dans le fichier
void Formule::dpll(string fout){
    int res = 0;
    pair<int,bool> choice;
    while(res==0){
        choice = choose();
		res = evol(get<0>(choice),get<1>(choice), false);
    }
    if(res==1){
        cout << "s SATISFIABLE" << endl;
		set<int> s;
		b.variables(s);
        for(int x:s){
            cout << x << " ";
        }
        cout << 0 << endl;
    }
    if(res==2)
        cout << "s UNSATISFIABLE" << endl;
}
