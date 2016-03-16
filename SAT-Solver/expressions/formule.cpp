#include <iostream>

#include "formule.hpp"

Formule::Formule(Expr& e){
	value = toEns(e);
	/*for (set<reference_wrapper<Expr>> x: *value){
        for (Expr& y : x)
            cout << y.to_string() << " ";
        cout << endl;
	}*/
	fixed = new map<int,int>;
	activeClauses = new set<int>();
	for (int i=0; i<value->size(); ++i){
		activeClauses->insert(i);
	}
}

Formule::Formule(vector<set<int>>* val){
	value = val;
	/*for (set<reference_wrapper<Expr>> x: *value){
        for (Expr& y : x)
            cout << y.to_string() << " ";
        cout << endl;
	}*/
	fixed = new map<int,int>;
	activeClauses = new set<int>();
	for (int i=0; i<value->size(); ++i){
		activeClauses->insert(i);
	}
}
//0:continue, 1:succeed, 2:fail, -1 backtrack
int Formule::evol(int var, bool val, bool forced){
    int res=0;
	set<int>* clauses_sup = new set<int>();
	set<int>* clauses_ret = new set<int>();
	set<pair<int,bool> > forcedVariables;
	set<int> activeClausesCopy = *activeClauses;
	if ((*fixed)[var]!=0){
        if (((*fixed)[var]==2) != val or !forced){
            if(b.back(value, activeClauses, fixed, &var, &val)){
                res = evol(var, val, true);
                if(res<=0)
                    return -1;
                else
                    return res;
            }
            else
                return 2;
        }else
            return 0;
	}
	(*fixed)[var]=(val?2:1);
	for (int c:activeClausesCopy){
		int e = (val?var:-var);
		set<int>::iterator p = (*value)[c].find(e);
		if(p != (*value)[c].end()){
            // TODO : p->get().del();
            clauses_ret->insert(c);
			(*value)[c].erase(p);
			if ((*value)[c].empty()){
                b.push(var,val,forced,clauses_sup,clauses_ret);
                if(b.back(value, activeClauses, fixed, &var, &val)){
                    int res = evol(var, val, true);
                    if(res<=0)
                        return -1;
                    else
                        return res;
                }
                else
                    return 2;
			}
			if ((*value)[c].size()==1){
				int varsol;
                if ((varsol = *(*value)[c].begin())>0)
                    forcedVariables.insert(pair<int,bool>(varsol,true));
				else
					forcedVariables.insert(pair<int,bool>(varsol,false));
			}
		}
		if (val)
			e = (val?var:-var);
		p = (*value)[c].find(e);
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
        res = evol(get<0>(x),get<1>(x),true);
        if(res!=0){
            return res;
        }
    }
	return 0;
}

pair<int,bool> Formule::choose() {
    int e = *((*value)[*(activeClauses->begin())].begin());
	if(e<0)
        return pair<int,bool>(-e,false);
	else
        return pair<int,bool>(e,true);
}

void Formule::dpll(string fout){
    int res = 0;
    pair<int,bool> choice;
    while(res<=0){
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
    if(res==2){
        cout << "s UNSATISFIABLE" << endl;
    }
}


int Formule::propage(int var, bool val){
	
}

int Formule::preTrait(){
	list<int> l;
	for (int i:*activeClauses){
		if ((*value)[i].size() == 1){
			int v = 0;
		} else {
			for (auto& v:(*value)[i]){
				int e = v;
				if (find((*value)[i].begin(), (*value)[i].end(), e) != (*value)[i].end())
					l.push_back(i);
			}
		}
	}
	for (int i:l){
		activeClauses->erase(i);
	}
	return -1;
}











































