#include <iostream>

#include "formule.hpp"

Formule::Formule(Expr& e){
	value = new vector<set<reference_wrapper<Expr>>>();
	e.toEns(value);
	activeClauses = new set<int>();
	for (int i=0; i<value.size; ++i){
		activeClauses.insert(i);
	}
}
//0:continue, 1:succeed, 2:fail
int Formule::evol(int var, bool val, bool forced){
	set<int>* clauses_sup = new set<int>();
	set<int>* clauses_ret = new set<int>();
	set<pair<int,bool> > forcedVariables;
	for (int c:*activeClauses){
		Expr e = val?ENot(EVar(var)):EVar(var);
		set<reference_wrapper<Expr> >::iterator p = *value[c].find(e);
		if(p != set::end){
            p->get().del();
            clauses_ret->insert(c);
			*value[c].erase(p);
			if (*value[c].empty()){
                b.push(var,val,forced,clauses_sup,clauses_ret);
                if(b.back(value, activeClauses))
                    return 0;
                else
                    return 2;
			}
			if(*value[c].size()==1){
                if(*(*value[c].begin()).type()=="EVar"){
                    forcedVariables.insert(pair<int,bool>(*(*value[c].begin()).getEtiq(),true));
                else
                    forcedVariables.insert(pair<int,bool>(*(*value[c].begin()).getOp().getEtiq(),false));
			}
		}
		e = val?EVar(var):ENot(EVar(var));
		p = *value[c].find(e);
		if(p != set::end){
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
        int r = this.evol(get<0>(x),get<1>(x),true);
        if(r!=0)
            return r;
    }
	return 0;
}
	
pair<int,bool> Formule::choose() {
    Expr& e = *(*value[*(activeClauses->begin())].begin());
    if(e.type()=="ENot")
        return pair<int,bool>(e.getOp().getValue(),false);
    else
        return pair<int,bool>(e.getValue(),true);
}
//TODO : écrire dans le fichier
void Formule::dpll(string fout){
    int res = 0;
    pair<int,bool> choice;
    while(r==0){
        choice = this.choose();
        this.evol(get<0>(choice),get<1>(choice))
    }
    if(r==1){
        cout << "s SATISFIABLE";
        for(x:b){
            cout << " " << x<1>?x<0>:-x<0>;
        }
        cout << 0 << endl;
    }
    ir(r==2)
        cout << "s UNSATISFIABLE" << endl;
}
