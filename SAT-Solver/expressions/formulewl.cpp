#include <iostream>

#include "formulewl.hpp"

vector<pair<int,int> > currentLvlLit;
extern int maxVar;
extern bool bcl;
extern bool bInterac;
bool forcedVariables;
int t = 1;

Formulewl::Formulewl(Expr& e, int heur):heuristique(heur){
    //cout << e.to_string() << endl;
	/*for (set<int> x: value){
        for (int y : x)
            cout << y << " ";
        cout << endl;
	}*/
	int nbVar=0;
	for(auto& x:*toEns()){
        value.emplace_back();
        for(int i:x){
            value.back().push_back(i);
            if(abs(i)>nbVar)
                nbVar=abs(i);
        }
	}
	nbApparPos = vector<int>(nbVar+1,0);
	nbApparNeg = vector<int>(nbVar+1,0);
	for(auto& x:value){
        for(int i:x){
            if(i>0)
                ++(nbApparPos[i]);
            else
                ++(nbApparNeg[-i]);
        }
	}
	for(int i=0; i<value.size();++i){
        for(int v:value.at(i)){
            appar[v].insert(i);
        }
	}
	for (int i=0; i<value.size(); ++i){
		activeClauses.insert(i);
		watched1[i]=0;
		watched2[i]=value[i].size()-1;
	}
}

Formulewl::Formulewl(vector<set<int>>& val, int heur):heuristique(heur){
	value = val;
	/*for (set<int> x: value){
        for (int y : x)
            cout << y << " ";
        cout << endl;
	}*/
	int nbVar=0;
	for(auto& x:val){
        value.emplace_back();
        for(int i:x){
            value.back().push_back(i);
            if(abs(i)>nbVar)
                nbVar=abs(i);
        }
	}
	nbApparPos = vector<int>(nbVar+1,0);
	nbApparNeg = vector<int>(nbVar+1,0);
	for(auto& x:value){
        for(int i:x){
            if(i>0)
                ++(nbApparPos[i]);
            else
                ++(nbApparNeg[-i]);
        }
	}
	for(int i=0; i<value.size();++i){
        for(int v:value.at(i)){
            appar[v].insert(i);
        }
	}
	for (int i=0; i<value.size(); ++i){
		activeClauses.insert(i);
	}
}
//0:continue, 1:succeed, 2:fail, -1 backtrack
int Formulewl::evol(bool& forcedVariables){
    int res=0;
	set<int> activeClausesCopy(activeClauses);
	for (int c:activeClausesCopy){
        while(watched1[c]<value[c].size() and fixed[-value[c][watched1[c]]]){
            ++watched1[c];
        }
        while(watched2[c]>=0 and fixed[-value[c][watched2[c]]]){
            --watched2[c];
        }
        if(fixed[value[c][watched1[c]]] or fixed[value[c][watched2[c]]]){
            activeClauses.erase(c);
        }
        if(watched1[c]==watched2[c]){
            forcedVariables.push(value[watched1[c]]);
            currentLvlLit.push_back(value[watched1[c]]);
            fixed[value[watched1[c]]]=t;
            forcedVariables=true;
        }
        if(watched1[c]>watched2[c]){
            if(t==0){
                return 2;
            }
            else{
                return -1;
            }
        }
	}
	return 0;
        /*
		if(p != value[c].end()){
            if(-var>0)
                --(nbApparPos[-var]);
            else
                --(nbApparNeg[var]);
            // TODO : p->get().del();
			value[c].erase(p);
			if (value[c].empty()){
                b.push(var,forced,clauses_sup,&(appar[-var]));
//cout << "ON BACK A CAUSE DE LA CLAUSE " << c+1 << endl;
                currentLvlLit.emplace_back(0,c);
                if(b.back(value, activeClauses, fixed, &var,nbApparPos,nbApparNeg)){
                    return -1;
                }
                else
                    return 2;
			}
		}
		p = value[c].find(var);
		if(p != value[c].end()){
            for(int i:value[c]){
                if(i>0)
                    --(nbApparPos[i]);
                else
                    --(nbApparNeg[-i]);
                if(!bcl and nbApparPos[abs(i)]+nbApparNeg[abs(i)]!=0 and !fixed[i] and !fixed[-i]){
                    if(nbApparPos[abs(i)]==0){
//cout << "ON FORCE " << -abs(i) << " QUI N'EST QUE NEGATIF" << endl;
                        if(!fixed[-abs(i)]){
                            fixed[-abs(i)]=t;
                            forcedVariables.push(-abs(i));
                        }
                    }
                    if(nbApparNeg[abs(i)]==0){
//cout << "ON FORCE " << abs(i) << " QUI N'EST QUE POSITIF" << endl;
                        if(!fixed[abs(i)]){
                            fixed[abs(i)]=t;
                            forcedVariables.push(abs(i));
                        }
                    }
                }
            }
            activeClauses.erase(c);
            clauses_sup->insert(c);
        }
        if (value[c].size()==1){
            if(!fixed[*value[c].begin()]){
//cout << "ON FORCE " << *value[c].begin() << " DANS " << c+1 << " (etape " << t << ")" << endl;
                forcedVariables.push(*value[c].begin());
                fixed[*value[c].begin()]=t;
                currentLvlLit.emplace_back(*value[c].begin(),c);
            }
        }
		if (activeClauses.empty()){
            b.push(var,forced,clauses_sup,&(appar[-var]));
            return 1;
		}
	}
	b.push(var,forced,clauses_sup,&(appar[-var]));
	return 0;*/
}

int Formulewl::choose() {
	int var;
	switch (heuristique) {
		case STANDARD:
			var = *(value[*(activeClauses.begin())][watched1[*(activeClauses.begin())]);
			break;
		case RAND:{
			srand(time(NULL));
			int c = rand()%activeClauses.size();
			auto it = activeClauses.begin();
			advance(it,c);
			c = *it;
			auto it2 = value[c].begin();
			var = rand()%value[c].size();
			advance(it2, var);
			var = *it2;
			break;
        }
		case MOMS:{
			int mini=INT_MAX;
			for (int c:activeClauses)
				if (value[c].size()<mini)
					mini=value[c].size();
            map<int,int> occur;
			for (int c:activeClauses)
				if (value[c].size()==mini){
					for (int v:value[c])
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
			for (int i=0; i<nbApparNeg.size(); ++i)
				if (nbApparNeg[i]>maxi and !fixed[i] and !fixed[-i]){
					maxi = nbApparNeg[i];
					var = -i;
				}
			for (int i=0; i<nbApparPos.size(); ++i)
				if (nbApparPos[i]>maxi and !fixed[i] and !fixed[-i]){
					maxi = nbApparPos[i];
					var = i;
				}
			break;
        }
		case VSIDS:{

			break;
		}
        default:
			break;
	}
    return var;
}

void Formulewl::dpll(string fout){
    long cpt=0;
    int res = 0;
    int choice;
    res=preTrait(forcedVariables);
    while(res<=0 ){
        if(res<=0){
            if(!forcedVariables){
                choice = choose();
                ++t;
                fixed[choice]=t;
                currentLvlLit.emplace_back(choice,-1);
//cout << choice << "  UN CHOIX" << endl;
                res = evol(forcedVariables);
            }
            else{
//cout << choice << "  FORCE" << endl;
                res = evol(forcedVariables);
            }
            while(res==-1){
                vector<int> toDel;
                if(bcl){
                    //creer graphe
                    value.push_back(set<int>());
                    watched1[value.size()-1]=0;
                    watched2[value.size()-1]=0;
                    set<int> litConflict;
                    set<int> litSeen;
                    vector<pair<int,int> > edges;
                    for(int v:value.at((*currentLvlLit.rbegin()).second)){
                        if(fixed[-v]==t and !litSeen.count(-v)){
                            litConflict.insert(-v);
                            litSeen.insert(-v);
                        }
                        else if(fixed[-v]!=t and !litSeen.count(-v)){
                            value.back().insert(v);
                            ++watched2[value.size()-1];
                            appar[-v].insert(value.size()-1);
                            litSeen.insert(-v);
                        }
                        edges.emplace_back(-v,0);
                    }
                    currentLvlLit.pop_back();
                    for(auto it = next(currentLvlLit.rbegin());it!=currentLvlLit.rend() and fixed[it->first]==t and litConflict.size()!=1;++it){
                        if(litConflict.count(it->first)){
                            litConflict.erase(it->first);
                            if(it->second!=-1){
                                for(int v:value.at(it->second)){
                                    if(v!=it->first){
                                        if(it->first!=0){
                                            edges.emplace_back(-v,it->first);
                                        }
                                        if(!fixed[-v]==t and !litSeen.count(-v)){
                                            litConflict.insert(-v);
                                            litSeen.insert(-v);
                                        }
                                        else if(fixed[-v]!=t and !litSeen.count(-v)){
                                            value.back().insert(v);
                                            ++watched2[value.size()-1];
                                            appar[v].insert(value.size()-1);
                                            litSeen.insert(-v);
                                        }
                                        litConflict.erase(it->first);
                                    }
                                }
                            }
                            else{
                                if(b.lastBack!=it->first)
                                    edges.emplace_back(b.lastBack,it->first);
                                litConflict.erase(it->first);
                                litConflict.insert(b.lastBack);
                            }
                        }
                    }
                    activeClauses.insert(value.size()-1);
                    value.back().insert(-*(litConflict.begin()));
                    ++watched2[value.size()-1];
                    appar[-*(litConflict.begin())].insert(value.size()-1);

                    while(!currentLvlLit.empty() and fixed[currentLvlLit.back().first]==t){
                        toDel.push_back(currentLvlLit.back().first);
                        choice = -currentLvlLit.back().first;
                        currentLvlLit.pop_back();
                    }
                    if(bInterac){
                        pause(edges,*(litConflict.begin()));
                    }
                }
                else{
                    while(!currentLvlLit.empty() and fixed[currentLvlLit.back().first]==t){
                        toDel.push_back(currentLvlLit.back().first);
                        choice = -currentLvlLit.back().first;
                        currentLvlLit.pop_back();
                    }
                }
//cout << "______________BACK" << endl;
                backtrack(toDel);
                --t;
//cout << "lastback : " << b.lastBack << endl;
                fixed[choice]=t;
                currentLvlLit.emplace_back(choice,-1);
//cout << choice << "  FORCE" << endl;
                res = evol(forcedVariables);
            }
        }
        forcedVariables=false;
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
int Formulewl::propage(int var){
	list<int> l;
	set<int>::iterator pos;
	for (int i:activeClauses){
        if (value[i].find(var) != value[i].end()){
			l.push_back(i);
		}else if ((pos = value[i].find(-var)) != value[i].end()){
			value[i].erase(pos);
			if (value[i].size() == 0)
				return 0;
			else if (value[i].size() == 1)
				if (!propage(*value[i].begin()))
					return 0;
		}
	}
	for (int sup:l)
		activeClauses.erase(sup);
	return 1;
}

//return values :
//0:continue, 1:succeed, 2:fail, -1 backtrack
int Formulewl::preTrait(queue<int>& forcedVariables){
	int res;
	bool nFini = true;
	while(nFini){
        nFini = false;
        set<int> activeClausesCopy(activeClauses);
        for (int i:activeClausesCopy){
            if (value[i].size() == 1){
                nFini = true;
                int v = *value[i].begin();
                forcedVariables.push(v);
                while(!forcedVariables.empty()){
                    v = forcedVariables.front();
                    forcedVariables.pop();
                    if ((res = evol(v, true, forcedVariables))>0)
                        return res;
                }
            } else {
                for (int v:value[i]){
                    if (value[i].find(-v) != value[i].end()){
                        activeClauses.erase(i);
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
	return activeClauses.empty();
}

//return values :
//0:continue, 1:succeed, 2:fail, -1 backtrack
int Formulewl::polUnique(queue<int>& forcedVariables){
    for(int i = 1; i<nbApparNeg.size();++i){
        if(nbApparPos[i]+nbApparNeg[i]!=0 and !fixed[i] and !fixed[-i]){
            int res;
            if(nbApparPos[i]==0)
                if((res = evol(-i,true,forcedVariables))!=0)
                    return res;
            if(nbApparNeg[i]==0)
                if((res = evol(i,true,forcedVariables))!=0)
                    return res;
        }
	}
	return 0;
}

//dot -Tpdf foo.dot -o foo.pdf
void Formulewl::graphe(vector<pair<int,int>>& edges, int uid){
	ofstream fic ("tmp.dot",ofstream::trunc);
	fic << "digraph G{" << endl;
	map<int,bool> defined;
	string bleu = ".56 .79 .96";
	string violet = ".78 .79 .96";
	fic << "\t0 [label=\"Conflict\", color=red, style=filled];" << endl;
	defined[0] = true;
	fic << "\t" << uid << "[color=yellow, style=filled];" << endl;
	defined[uid] = true;
	for (pair<int,int>& e:edges){
		if (!defined[e.first]){
			fic << "\t" << e.first << " [color=\"" << (fixed[e.first]?violet:bleu) << "\", style=filled];" << endl;
			defined[e.first] = true;
		}
		if (!defined[e.second]){
			fic << "\t" << e.second << " [color=" << bleu << ", style=filled];" << endl;
			defined[e.second] = true;
		}
		fic << "\t" << e.first << " -> " << e.second << " ;" << endl;
	}
	fic << "}" << endl;
	fic.close();
	system("dot -Tpdf tmp.dot -o tmp.pdf");
	system("evince -f tmp.pdf");
	system("rm tmp.dot");
	system("rm tmp.pdf");
}

void Formulewl::pause(vector<pair<int,int>>& edges, int uid){
	cout << "g : dessiner le graphe des conflits" << endl;
	cout << "c : continuer jusqu'au prochain conflit" << endl;
	cout << "t : finir le déroulement de dpll sans intéruption" << endl;
	char rep;
debut:
	scanf("%c",&rep);
	switch(rep){
		case 'g':
			graphe(edges,uid);
			cout << "c : continuer jusqu'au prochain conflit" << endl;
			cout << "t : finir le déroulement de dpll sans intéruption" << endl;
		sousdebut:
			scanf("%c",&rep);
			switch(rep){
				case 'c':
					return;
				case 't':
					bInterac=false;
					return;
				default:
					goto sousdebut;
			}
		case 'c':
			return;
		case 't':
			bInterac=false;
			return;
		default:
			cout << "Réponse incorrecte." << endl;
			goto debut;
	}

}



































