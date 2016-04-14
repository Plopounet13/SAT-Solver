#include <iostream>

#include "formule.hpp"
#include <unordered_set>

vector<pair<int,int> > currentLvlLit;

Formule::Formule(Expr& e, int heur):heuristique(heur){
    cout << e.to_string() << endl;
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
                ++((*nbApparPos)[i]);
            else
                ++((*nbApparNeg)[-i]);
        }
	}
	fixed = new map<int,bool>;
	activeClauses = new set<int>();
	for (int i=0; i<value->size(); ++i){
		activeClauses->insert(i);
	}
}

Formule::Formule(vector<set<int>>* val, int heur):heuristique(heur){
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
	nbApparPos = new vector<int>(nbVar+1,0);
	nbApparNeg = new vector<int>(nbVar+1,0);
	for(auto& x:*value){
        for(int i:x){
            if(i>0)
                ++((*nbApparPos)[i]);
            else
                ++((*nbApparNeg)[-i]);
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
    int res=0;
	set<int>* clauses_sup = new set<int>();
	set<int>* clauses_ret = new set<int>();
	set<int> activeClausesCopy(*activeClauses);
	(*fixed)[var]=true;
	for (int c:activeClausesCopy){
		set<int>::iterator p = (*value)[c].find(-var);
		if(p != (*value)[c].end()){
            if(-var>0)
                --((*nbApparPos)[-var]);
            else
                --((*nbApparNeg)[var]);
            // TODO : p->get().del();
            clauses_ret->insert(c);
			(*value)[c].erase(p);
			if ((*value)[c].empty()){
                b.push(var,forced,clauses_sup,clauses_ret);
//cout << "ON BACK A CAUSE DE LA CLAUSE " << c+1 << endl;
                if(b.back(value, activeClauses, fixed, &var,nbApparPos,nbApparNeg)){
                    return -1;
                }
                else
                    return 2;
			}
			if ((*value)[c].size()==1){
//cout << "ON FORCE " << *(*value)[c].begin() << " DANS " << c+1 << endl;
                if(!(*fixed)[*(*value)[c].begin()]){
                    forcedVariables.push(*(*value)[c].begin());
                    (*fixed)[*(*value)[c].begin()]=false;
                    currentLvlLit.emplace_back(*(*value)[c].begin(),c);
                }
			}
		}
		p = (*value)[c].find(var);
		if(p != (*value)[c].end()){
            if(!bcl){
                for(int i:(*value)[c]){
                    if(i>0)
                        --((*nbApparPos)[i]);
                    else
                        --((*nbApparNeg)[-i]);
                    if((*nbApparPos)[abs(i)]+(*nbApparNeg)[abs(i)]!=0 and !(*fixed)[i] and !(*fixed)[-i]){
                        if((*nbApparPos)[abs(i)]==0){
//cout << "ON FORCE " << forcedVariables.back() << " QUI N'EST QUE DANS " << c+1 << endl;
                            if(!fixed[-abs(i)]){
                                fixed[-abs(i)]=true;
                                forcedVariables.push(-abs(i));
                            }
                        }
                        if((*nbApparNeg)[abs(i)]==0){
//cout << "ON FORCE " << forcedVariables.back() << " QUI N'EST QUE DANS " << c+1 << endl;
                            if(!fixed[-abs(i)]){
                                fixed[-abs(i)]=true;
                                forcedVariables.push(abs(i));
                            }
                        }
                    }
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
	b.push(var,forced,clauses_sup,clauses_ret);
	return 0;
}

int Formule::choose() {
	int var;
	switch (heuristique) {
		case STANDARD:
			var = *((*value)[*(activeClauses->begin())].begin());
			break;
		case RAND:{
			srand(time(NULL));
			int c = rand()%activeClauses->size();
			auto it = activeClauses->begin();
			advance(it,c);
			c = *it;
			auto it2 = (*value)[c].begin();
			var = rand()%(*value)[c].size();
			advance(it2, var);
			var = *it2;
			break;
        }
		case MOMS:{
			int mini=INT_MAX;
			for (int c:*activeClauses)
				if ((*value)[c].size()<mini)
					mini=(*value)[c].size();
            map<int,int> occur;
			for (int c:*activeClauses)
				if ((*value)[c].size()==mini){
					for (int v:(*value)[c])
						++occur[v];
				}
			int maxi=0;
			for (auto& p:occur){
				if (p.second>maxi){
					var = p.first;
				}
			}
			break;
        }
		case DLIS:{
			int maxi=0;
			for (int i=0; i<nbApparNeg->size(); ++i)
				if ((*nbApparNeg)[i]>maxi){
					maxi = (*nbApparNeg)[i];
					var = -i;
				}
			for (int i=0; i<nbApparPos->size(); ++i)
				if ((*nbApparPos)[i]>maxi){
					maxi = (*nbApparPos)[i];
					var = i;
				}
			break;
        }
        default:
			break;
	}
    return var;
}

void Formule::dpll(string fout){
    long cpt=0;
    int res = 0;
    int choice;
    queue<int> forcedVariables;
    res=preTrait(forcedVariables);
    initial_value = new vector<set<int>>(*value);
    while(res<=0){
        if(res<=0){
            if(forcedVariables.empty()){
                currentLvlLit.clear();
                choice = choose();
                currentLvlLit.emplace_back(choice,-1);
//cout << choice << "  UN CHOIX" << endl;
                res = evol(choice, false, forcedVariables);
            }
            else{
                choice = forcedVariables.front();
//cout << choice << "  FORCE" << endl;
                forcedVariables.pop();
                res = evol(choice, true, forcedVariables);
            }
            while(res==-1){
//cout << "______________BACK" << endl;
                while(!forcedVariables.empty()){
                    (*fixed)[forcedVariables.front()]=false;
                    forcedVariables.pop();
                }
                currentLvlLit.clear();
                choice = b.lastBack;
                currentLvlLit.emplace_back(choice,-1);
//cout << -choice << "  FORCE" << endl;
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
			l.push_back(i);
		}else if ((pos = (*value)[i].find(-var)) != (*value)[i].end()){
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
                forcedVariables.push(v);
                while(!forcedVariables.empty()){
                    v = forcedVariables.front();
                    forcedVariables.pop();
                    if ((res = evol(v, true, forcedVariables))>0)
                        return res;
                }
            } else {
                for (int v:(*value)[i]){
                    if ((*value)[i].find(-v) != (*value)[i].end()){
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


void Formule::pause(){
	cout << "g : dessiner le graphe des conflits" << endl;
	cout << "c : continuer jusqu'au prochain conflit" << endl;
	cout << "t : finir le dérouolement de dpll sans intéruption" << endl;
	char rep;
debut:
	cin >> rep;
	switch(rep){
		case 'g':
			graphe();
			cout << "c : continuer jusqu'au prochain conflit" << endl;
			cout << "t : finir le déroulement de dpll sans intéruption" << endl;
		sousdebut:
			cin >> rep;
			switch(rep){
				case 'c':
					return;
				case 't':
					bGraphe=false;
					return;
				default:
					goto sousdebut;
			}
		case 'c':
			return;
		case 't':
			bGraphe=false;
			return;
		default:
			cout << "Réponse incorrecte." << endl;
			goto debut;
	}

}



































