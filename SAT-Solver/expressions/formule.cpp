#include <iostream>

#include "formule.hpp"
#include <unordered_set>

Formule::Formule(Expr& e){
    //cout << e.to_string() << endl;
	value = toEns(e);
	/*for (set<int> x: *value){
        for (int y : x)
            cout << y << " ";
        cout << endl;
	}*/
	int nbVar=0;
	for(auto& x:*value){
        for(int i:x)
            if(abs(i)>nbVar)
                nbVar=abs(i);
	}
	nbApparPos = new vector<int>(nbVar+1,0);
	nbApparNeg = new vector<int>(nbVar+1,0);
	for(auto& x:*value){
        for(int i:x){
            if(i>0)
                ++(*nbApparPos)[i];
            else
                ++(*nbApparNeg)[-i];
        }
	}
	fixed = new map<int,bool>;
	activeClauses = new set<int>();
	for (int i=0; i<value->size(); ++i){
		activeClauses->insert(i);
	}
}

Formule::Formule(vector<set<int>>* val){
	value = val;
	/*for (set<int> x: *value){
        for (int y : x)
            cout << y << " ";
        cout << endl;
	}*/
	int nbVar=0;
	for(auto& x:*value){
        for(int i:x)
            if(abs(i)>nbVar)
                nbVar=abs(i);
	}
	nbApparPos = new vector<int>(nbVar,0);
	nbApparNeg = new vector<int>(nbVar,0);
	for(auto& x:*value){
        for(int i:x){
            if(i>0)
                ++(*nbApparPos)[i];
            else
                ++(*nbApparNeg)[-i];
        }
	}
	fixed = new map<int,bool>;
	activeClauses = new set<int>();
	for (int i=0; i<value->size(); ++i){
		activeClauses->insert(i);
	}
}
//0:continue, 1:succeed, 2:fail, -1 backtrack
int Formule::evol(int var, bool forced, queue<int>& forcedVariables){
//cout << var << "  OPEN" << endl;
    int res=0;
	set<int>* clauses_sup = new set<int>();
	set<int>* clauses_ret = new set<int>();
	set<int> activeClausesCopy;//(*activeClauses);
for(int i:*activeClauses){
//cout << i << endl;
activeClausesCopy.insert(i);
//cout << i << endl;
}
	if ((*fixed)[var]==true){
        return 0;
	}
	if ((*fixed)[-var]==true){
            if(b.back(value, activeClauses, fixed, &var, nbApparPos, nbApparNeg))
                return -1;
            else
                return 2;
	}
	(*fixed)[var]=true;
	for (int c:activeClausesCopy){
		set<int>::iterator p = (*value)[c].find(-var);
		if(p != (*value)[c].end()){
            if(-var>0)
                --(*nbApparPos)[-var];
            else
                --(*nbApparNeg)[var];
            // TODO : p->get().del();
            clauses_ret->insert(c);
			(*value)[c].erase(p);
			if ((*value)[c].empty()){
                b.push(var,forced,clauses_sup,clauses_ret);
                if(b.back(value, activeClauses, fixed, &var,nbApparPos,nbApparNeg)){
                    return -1;
                }
                else
                    return 2;
			}
			if ((*value)[c].size()==1){
                forcedVariables.push(*(*value)[c].begin());
			}
		}
		p = (*value)[c].find(var);
		if(p != (*value)[c].end()){
            for(int i:(*value)[c]){
                if(i>0)
                    --(*nbApparPos)[i];
                else
                    --(*nbApparNeg)[-i];
                if((*nbApparPos)[i]+(*nbApparNeg)[i]!=0 and !(*fixed)[i] and !(*fixed)[-i]){
                    if((*nbApparPos)[abs(i)]==0)
                        forcedVariables.push(-abs(i));
                    if((*nbApparNeg)[abs(i)]==0)
                        forcedVariables.push(abs(i));
                }
            }
            activeClauses->erase(c);
            clauses_sup->insert(c);
        }
		if (activeClauses->empty()){
            b.push(var,forced,clauses_sup,clauses_ret);
            return 1;
		}
	}
//cout << 2 << endl;
	b.push(var,forced,clauses_sup,clauses_ret);
//cout << var << "  CLOS" << endl;
	return 0;
}

int Formule::choose() {
    int var = *((*value)[*(activeClauses->begin())].begin());
    return var;
}

void Formule::dpll(string fout){
    long cpt=0;
    int res = 0;
    int choice;
    queue<int> forcedVariables;
    res=this->preTrait(forcedVariables);
	/*for (int i: *activeClauses){
        for (int y : (*value)[i])
            cout << y << " ";
        cout << endl;
	}*/
    while(res<=0){
    /*++cpt;
    if((cpt%1000)==0)
    cout << cpt << endl;*/
        if(res<=0){
            if(forcedVariables.empty()){
                choice = choose();
//cout << choice << "  UN CHOIX" << endl;
                res = evol(choice, false, forcedVariables);
            }
            else{
                choice = forcedVariables.front();
                forcedVariables.pop();
                res = evol(choice, true, forcedVariables);
            }
            while(res==-1){
                while(!forcedVariables.empty())
                    forcedVariables.pop();
                choice = choose();
                res = evol(-choice,true,forcedVariables);
            }
        }
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
//TODO : ajouter au backtrack
//return values :
//	1 if success
//	0 if fails
int Formule::propage(int var){
	list<int> l;
	set<int>::iterator pos;
	for (int i:*activeClauses){
        if ((*value)[i].find(var) != (*value)[i].end()){
		//if (find((*value)[i].begin(), (*value)[i].end(), var) != (*value)[i].end()){
			l.push_back(i);
		}else if ((pos = (*value)[i].find(-var)) != (*value)[i].end()){
		//}else if ((pos = find((*value)[i].begin(), (*value)[i].end(), -var)) != (*value)[i].end()){
			(*value)[i].erase(pos);
			if ((*value)[i].size() == 0)
				return 0;
			else if ((*value)[i].size() == 1)
				if (!propage(*(*value)[i].begin()))
					return 0;
		}
	}
	for (int sup:l)
		activeClauses->erase(sup);
	return 1;
}

//return values :
//0:continue, 1:succeed, 2:fail, -1 backtrack
int Formule::preTrait(queue<int>& forcedVariables){
	int res;
	bool nFini = true;
	while(nFini){
        nFini = false;
        set<int> activeClausesCopy(*activeClauses);
        for (int i:activeClausesCopy){
            if ((*value)[i].size() == 1){
                nFini = true;
                int v = *(*value)[i].begin();
                if ((res=evol(v,true,forcedVariables))>0)
                    return res;
            } else {
                for (int v:(*value)[i]){
                    if ((*value)[i].find(-v) != (*value)[i].end()){
                    //if (find((*value)[i].begin(), (*value)[i].end(), -v) != (*value)[i].end()){
                        activeClauses->erase(i);
                        break;
                    }
                }
            }
        }
        while(!forcedVariables.empty()){
            int choice = forcedVariables.front();
            forcedVariables.pop();
            res = evol(choice, true, forcedVariables);
        }
    }
	return activeClauses->empty();
}

//return values :
//0:continue, 1:succeed, 2:fail, -1 backtrack
int Formule::polUnique(queue<int>& forcedVariables){
    for(int i = 1; i<nbApparPos->size();++i){
        if((*nbApparPos)[i]+(*nbApparNeg)[i]!=0 and !(*fixed)[i] and !(*fixed)[-i]){
            int res;
            if((*nbApparPos)[i]==0)
                if((res = evol(-i,true,forcedVariables))!=0)
                    return res;
            if((*nbApparNeg)[i]==0)
                if((res = evol(i,true,forcedVariables))!=0)
                    return res;
        }
	}
	return 0;
}









































