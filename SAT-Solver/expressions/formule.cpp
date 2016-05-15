#include <iostream>

#include "formule.hpp"

vector<pair<int,int> > currentLvlLit;
vector<pair<int,int> > currentLvlCl;
extern int maxVar;
extern bool bcl;
extern bool bInterac;
extern bool bForget;
extern bool bwl;
int nbVar=0;
int t;

void reset(vector<vector<int>::iterator>& watched1, vector<vector<int>::reverse_iterator>& watched2, vector<vector<int>>& valueWL){
    watched1.clear();
    watched2.clear();
    for(auto& c:valueWL){
        watched1.push_back(c.begin());
        watched2.push_back(c.rbegin());
    }
}
Formule::Formule(Expr& e, int heur):heuristique(heur),fixed(0,0), lockFixed(0,0){
    //cout << e.to_string() << endl;
    value = *toEns(e);
    if(bwl){
        for(auto& c:value){
            valueWL.emplace_back();
            for(int v:c){
                valueWL.back().push_back(v);
            }
        }
    }
	/*for (set<int> x: value){
        for (int y : x)
            cout << y << " ";
        cout << endl;
	}*/
	for(auto& x:value){
        for(int i:x)
            if(abs(i)>nbVar)
                nbVar=abs(i);
	}
	fixed = myv<int>(2*nbVar+1,nbVar);
	lockFixed = myv<mutex>(2*nbVar+1,nbVar);
	nbApparPos = vector<int>(nbVar+1,0);
	nbApparNeg = vector<int>(nbVar+1,0);
	lockNbAppar = vector<mutex>(nbVar+1);
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
	nbClauseInit = activeClauses.size();

///* WATCHED *///
    if(bwl){
        reset(watched1,watched2,valueWL);
    }
}

Formule::Formule(vector<unordered_set<int>>& val, int heur):heuristique(heur),fixed(2*maxVar+1,maxVar),lockFixed(2*maxVar+1,maxVar){
	value = val;
    if(bwl){
        for(auto& c:value){
            valueWL.emplace_back();
            for(int v:c){
                valueWL.back().push_back(v);
            }
        }
    }
	/*for (set<int> x: value){
        for (int y : x)
            cout << y << " ";
        cout << endl;
	}*/
	for(auto& x:value){
        for(int i:x)
            if(abs(i)>nbVar)
                nbVar=abs(i);
	}
	nbApparPos = vector<int>(nbVar+1,0);
	nbApparNeg = vector<int>(nbVar+1,0);
	lockNbAppar = vector<mutex>(nbVar+1);
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
	nbClauseInit = activeClauses.size();

///* WATCHED *///
    if(bwl){
        reset(watched1,watched2,valueWL);
    }
}

void Formule::retire(int c){
	lockActiveClauses.lock();
	if (activeClauses.erase(c))
		for (auto& v:value[c]){
			lockNbAppar[abs(v)].lock();
			if (v<0)
				--(nbApparNeg[-v]);
			else
				--(nbApparPos[v]);
			lockNbAppar[abs(v)].unlock();
		}
	lockActiveClauses.unlock();
}

void Formule::reduceAppar(int i){
	lockNbAppar[abs(i)].lock();
    if(i>0)
        --(nbApparPos[i]);
    else
		--(nbApparNeg[-i]);
	lockFixed[i].lock();
	lockFixed[-i].lock();
    if(!bcl and nbApparPos[abs(i)]+nbApparNeg[abs(i)]!=0 and !fixed[i] and !fixed[-i]){
        if(nbApparPos[abs(i)]==0){
//cout << "ON FORCE " << -abs(i) << " QUI N'EST QUE NEGATIF" << endl;
            if(!fixed[-abs(i)]){
                fixed[-abs(i)]=t;
                forcedVariables.push(-abs(i));
                currentLvlLit.emplace_back(-abs(i),0);
            }
        }
        if(nbApparNeg[abs(i)]==0){
//cout << "ON FORCE " << abs(i) << " QUI N'EST QUE POSITIF" << endl;
            if(!fixed[abs(i)]){
                fixed[abs(i)]=t;
                forcedVariables.push(abs(i));
                currentLvlLit.emplace_back(abs(i),0);
            }
        }
	}
	lockFixed[i].unlock();
	lockFixed[-i].unlock();
	lockNbAppar[abs(i)].unlock();
}

void* Formule::boucleThread(set<int>::iterator& start, set<int>::iterator& end, queue<int>& forcedVariables){
	for(auto& c=start; c!=end; ++c){
		while(watched1[*c] != valueWL[*c].end() and fixed[-*watched1[*c]]){
			/// à supprimer
			reduceAppar(*watched1[*c]);
			++watched1[*c];
		}
		///* On vérifie si c est une clause fausse *///
		if(watched1[*c] == valueWL[*c].end()){
			currentLvlLit.emplace_back(0,*c);
			//cout << "ON BACK A CAUSE DE LA CLAUSE " << c+1 << endl;
			if(t!=1)
				return -1;
			else
				return 2;
		}
		///* Actualisation de watched2[c] *///
		while(fixed[-*watched2[*c]]){
			/// à supprimer
			reduceAppar(*watched2[*c]);
			++watched2[*c];
		}
		///* On vérifie si c est une clause vraie *///
		if(fixed[*watched1[*c]] or fixed[*watched2[*c]]){
			clausesToDel.push_back(*c);
			vector<int>::iterator it=watched1[*c];
			for(; it!=watched2[c].base()-1; ++it){
				reduceAppar(*it);
			}
			reduceAppar(*it);
		}
		///* On vérifie si c est une clause unitaire *///
		else if(watched1[*c] == watched2[*c].base()-1){
			if (bForget){
				if(scoreForget.count(*c))
					scoreForget[*c]+=value.size();
				for (auto& s:scoreForget){
					if (s.first != *c)
						--s.second;
					if (s.second <= 0)
						retire(s.first);
				}
			}
			//cout << "ON FORCE " << *watched1[c] << " DANS " << c+1 << " (etape " << t << ")" << endl;
			forcedVariables.push(*watched1[*c]);
			fixed[*watched1[*c]]=t;
			currentLvlLit.emplace_back(*watched1[*c],*c);
			reduceAppar(*watched1[*c]);
		}
	}
}

int Formule::evolWL(bool forced, queue<int>& forcedVariables){
    vector<int> clausesToDel;
	for (int c:activeClauses){
///* Actualisation de watched1[c] *///
        while(watched1[c] != valueWL[c].end() and fixed[-*watched1[c]]){
/// à supprimer
            reduceAppar(*watched1[c]);
            ++watched1[c];
        }
///* On vérifie si c est une clause fausse *///
        if(watched1[c] == valueWL[c].end()){
            currentLvlLit.emplace_back(0,c);
//cout << "ON BACK A CAUSE DE LA CLAUSE " << c+1 << endl;
            if(t!=1)
                return -1;
            else
                return 2;
        }
///* Actualisation de watched2[c] *///
        while(fixed[-*watched2[c]]){
/// à supprimer
            reduceAppar(*watched2[c]);
            ++watched2[c];
        }
///* On vérifie si c est une clause vraie *///
        if(fixed[*watched1[c]] or fixed[*watched2[c]]){
            clausesToDel.push_back(c);
            vector<int>::iterator it=watched1[c];
            for(; it!=watched2[c].base()-1; ++it){
                reduceAppar(*it);
            }
            reduceAppar(*it);
        }
///* On vérifie si c est une clause unitaire *///
        else if(watched1[c] == watched2[c].base()-1){
            if (bForget){
                if(scoreForget.count(c))
                    scoreForget[c]+=value.size();
                for (auto& s:scoreForget){
                    if (s.first != c)
                        --s.second;
                    if (s.second <= 0)
                        retire(s.first);
                }
            }
//cout << "ON FORCE " << *watched1[c] << " DANS " << c+1 << " (etape " << t << ")" << endl;
            forcedVariables.push(*watched1[c]);
            fixed[*watched1[c]]=t;
            currentLvlLit.emplace_back(*watched1[c],c);
            reduceAppar(*watched1[c]);
        }
	}
	for(int c:clausesToDel){
        activeClauses.erase(c);
        currentLvlCl.emplace_back(c,t);
    }
    if (activeClauses.empty()){
        return 1;
    }
	return 0;
}

//0:continue, 1:succeed, 2:fail, -1 backtrack
int Formule::evol(int var, bool forced, queue<int>& forcedVariables){

///* WATCHED *///
    if(bwl){
        while(!forcedVariables.empty()){
            forcedVariables.pop();
        }
        return evolWL(forced,forcedVariables);
    }

    bool bBack = false;
    int cBack;
	set<int>* clauses_sup = new set<int>();
	set<int>::iterator itP = appar[var].begin();
	set<int>::iterator endP = appar[var].end();

	set<int>::iterator itN = appar[-var].begin();
	set<int>::iterator endN = appar[-var].end();
    vector<int> clausesToDel;
	for (int c:activeClauses){
        while(itN!=endN and *itN < c)
            ++itN;
		if(itN!=endN and *itN == c){
            reduceAppar(-var);
            // TODO : p->get().del();
			value[c].erase(-var);
			if (value[c].empty()){
                bBack=true;
                cBack = c;
/*                clauses_sup->clear();
                b.push(var,forced,clauses_sup,&(appar[-var]));
cout << "ON BACK A CAUSE DE LA CLAUSE " << c+1 << endl;
                currentLvlLit.emplace_back(0,c);
                if(b.back(value, activeClauses, fixed, &var,nbApparPos,nbApparNeg))
                    return -1;
                else
                    return 2;*/
			}
		}
        while(itP!=endP and *itP < c)
            ++itP;
		if(itP!=endP and *itP == c){
            for(int i:value[c]){
                reduceAppar(i);
            }
            clausesToDel.push_back(c);
            clauses_sup->insert(c);
        }
        if (value[c].size()==1){
            if(!fixed[*value[c].begin()]){
//cout << "ON FORCE " << *value[c].begin() << " DANS " << c+1 << " (etape " << t << ")" << endl;
				if (bForget){
					if(scoreForget.count(c))
						scoreForget[c]+=value.size();
					for (auto& s:scoreForget){
						if (s.first != c)
							--s.second;
						if (s.second <= 0)
							retire(s.first);
					}
				}
                forcedVariables.push(*value[c].begin());
                fixed[*value[c].begin()]=t;
                currentLvlLit.emplace_back(*value[c].begin(),c);
            }
        }
	}
	for(int c:clausesToDel){
        activeClauses.erase(c);
    }
    if (activeClauses.empty()){
        b.push(var,forced,clauses_sup,&(appar[-var]));
        return 1;
    }
	b.push(var,forced,clauses_sup,&(appar[-var]));
	if(bBack){
//cout << "ON BACK A CAUSE DE LA CLAUSE " << cBack+1 << endl;
        currentLvlLit.emplace_back(0,cBack);
        if(b.back(value, activeClauses, fixed, &var,nbApparPos,nbApparNeg))
            return -1;
        else
            return 2;
    }
	return 0;
}

int Formule::chooseWL() {
	int var;
	switch (heuristique) {
		case STANDARD:
			var = *watched1[*activeClauses.begin()];
			break;
		case RAND:{
			srand(time(NULL));
			int c = rand()%activeClauses.size();
			auto it = activeClauses.begin();
			advance(it,c);
			c = *it;
			var = *watched1[c];
			/*auto it2 = watched1[c];
			var = rand()%(distance(watched1[c],watched2[c])+1);
			advance(it2, var);
			var = *it2;*/
			break;
        }
		case MOMS:{
			int mini=INT_MAX;
			vector<int> clauses;
			for (int c:activeClauses){
                int d=distance(watched1[c],watched2[c].base());
				if (d<mini){
					mini=d;
					clauses.clear();
                }
                if (d==mini){
                    clauses.push_back(c);
                }
            }
            myv<int> occur(2*nbVar+1,nbVar);
            int maxi=0;
			for (int c:clauses){
                for (auto it=watched1[c]; it != watched2[c].base();++it){
                    int v= *it;
                    ++occur[v];
                    if(occur[v]>maxi and !fixed[v] and !fixed[-v]){
                        var = v;
                        maxi = occur[v];
                    }
                }
            }
			break;
        }
		case DLIS:{
			int maxi=0;
			for (int i=0; i<nbApparNeg.size(); ++i){
				if (nbApparNeg[i]>maxi and !fixed[i] and !fixed[-i]){
					maxi = nbApparNeg[i];
					var = -i;
				}
            }
			for (int i=0; i<nbApparPos.size(); ++i){
				if (nbApparPos[i]>maxi and !fixed[i] and !fixed[-i]){
					maxi = nbApparPos[i];
					var = i;
				}
            }
			break;
        }
		case VSIDS:{
			var = *watched1[*activeClauses.begin()];
			int maxi = scoreVsids[abs(var)];
			for (auto& p:scoreVsids){
				if (!fixed[p.first] && !fixed[-p.first] && p.second > maxi){
					maxi=p.second;
					var=p.first;
				}
				p.second=p.second/2;
			}
			break;
		}
        default:
			break;
	}
    return var;
}

int Formule::choose() {
    if(bwl){
        return chooseWL();
    }
	int var;
	switch (heuristique) {
		case STANDARD:
			var = *(value[*(activeClauses.begin())].begin());
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
            myv<int> occur(2*nbVar+1,nbVar);
            int maxi=0;
			for (int c:activeClauses)
				if (value[c].size()==mini){
					for (int v:value[c]){
						++occur[v];
						if(occur[v]>maxi){
                            var = v;
                            maxi = occur[v];
						}
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
			var = *(value[*(activeClauses.begin())].begin());
			int maxi = scoreVsids[abs(var)];
			for (auto& p:scoreVsids){
				if (!fixed[p.first] && !fixed[-p.first] && p.second > maxi){
					maxi=p.second;
					var=p.first;
				}
				p.second=p.second/2;
			}
			break;
		}
        default:
			break;
	}
    return var;
}

void Formule::dpll(string fout){
    t=1;
    long cpt=0;
    int res = 0;
    int choice;
    queue<int> forcedVariables;
    res=preTrait(forcedVariables);
    initial_value = value;
    vector<int> nbApparNegInit(nbApparNeg);
    vector<int> nbApparPosInit(nbApparPos);
    while(res<=0){
//cout << "Time " << t << endl;
        if(res<=0){
            if(forcedVariables.empty()){
                choice = choose();
                ++t;
                fixed[choice]=t;
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
                while(!bwl and !forcedVariables.empty()){
                    fixed[forcedVariables.front()]=0;
                    forcedVariables.pop();
                }
///* WATCHED *///
                if(bwl){
                    reset(watched1,watched2,valueWL);
                    nbApparNeg = nbApparNegInit;
                    nbApparPos = nbApparPosInit;
                }
                if(bcl){
                    //creer graphe
                    initial_value.push_back(unordered_set<int>());
                    value.push_back(unordered_set<int>());
                    if(bwl)
                        valueWL.emplace_back();
                    set<int> litConflict;
                    set<int> litSeen;
                    vector<pair<int,int> > edges;
                    if(bwl){
                        for(int v:initial_value.at((*currentLvlLit.rbegin()).second)){
                            if(fixed[-v]==t and !litSeen.count(-v)){
                                litConflict.insert(-v);
                                litSeen.insert(-v);
                            }
                            else if(fixed[-v]!=t){
                                initial_value.back().insert(v);
                                if(bwl)
                                    valueWL.back().push_back(v);
                                scoreVsids[abs(v)]+=INC_SCORE;
                                appar[v].insert(value.size()-1);
                            }
                            edges.emplace_back(-v,0);
                        }
                        pair<int,int> e = currentLvlLit.back();
                        currentLvlLit.pop_back();
                        int i = currentLvlLit.size()-1;
                        while(i!=-1 && (fixed[currentLvlLit[i].first]==t)){
                            fixed[currentLvlLit[i].first]=0;
                            choice=currentLvlLit[i].first;
                            --i;
                        }
                        currentLvlLit.push_back(e);
					}
					else{
                        for(int v:initial_value.at((*currentLvlLit.rbegin()).second)){
                            if(!fixed[-v] and !litSeen.count(-v)){
                                litConflict.insert(-v);
                                litSeen.insert(-v);
                            }
                            else if(fixed[-v]){
                                initial_value.back().insert(v);
                                scoreVsids[abs(v)]+=INC_SCORE;
                                appar[v].insert(value.size()-1);
                            }
                            edges.emplace_back(-v,0);
                        }
                        pair<int,int> e = currentLvlLit.back();
                        currentLvlLit.pop_back();
                        while(!currentLvlLit.empty() and fixed[currentLvlLit.back().first]==t){
                            fixed[currentLvlLit.back().first]=0;
                            currentLvlLit.pop_back();
                        }
                        choice = b.lastBack;
                        currentLvlLit.push_back(e);
					}
                    for(auto it = next(currentLvlLit.rbegin());it!=currentLvlLit.rend() and fixed[it->first]==0 and litConflict.size()!=1;++it){
                        if(litConflict.count(it->first)){
                            litConflict.erase(it->first);
                            if(it->second!=-1){
                                for(int v:initial_value[it->second]){
                                    if(v!=it->first){
                                        if(it->first!=0){
                                            edges.emplace_back(-v,it->first);
                                        }
                                        if(!fixed[-v] and !litSeen.count(-v)){
                                            litConflict.insert(-v);
                                            litSeen.insert(-v);
                                        }
                                        else if(fixed[-v]){
                                            initial_value.back().insert(v);
                                            if(v>0)
                                                ++nbApparPos[v];
                                            else
                                                ++nbApparNeg[-v];
                                            if(bwl){
                                                valueWL.back().push_back(v);
                                                if(v>0)
                                                    ++nbApparPosInit[v];
                                                else
                                                    ++nbApparNegInit[-v];
                                            }
											scoreVsids[abs(v)]+=INC_SCORE;
                                            appar[v].insert(value.size()-1);
                                        }
                                        litConflict.erase(it->first);
                                    }
                                }
                            }
                            else{
                                if(choice!=it->first)
                                    edges.emplace_back(choice,it->first);
                                litConflict.erase(it->first);
                                litConflict.insert(choice);
                            }
                        }
                    }
                    int v = -*(litConflict.begin());
                    activeClauses.insert(value.size()-1);
					initial_value.back().insert(v);
                    if(v>0)
                        ++nbApparPos[v];
                    else
                        ++nbApparNeg[-v];
                    if(bwl){
                        valueWL.back().push_back(v);
                        watched1.push_back(valueWL.back().begin());
                        watched2.push_back(valueWL.back().rbegin());
                        if(v>0)
                            ++nbApparPosInit[v];
                        else
                            ++nbApparNegInit[-v];
                    }
					scoreVsids[abs(v)]+=INC_SCORE;
                    value.back().insert(v);
                    appar[v].insert(value.size()-1);
					if (bForget)
						scoreForget[value.size()-1]=value.size();
                    int maxi_t = 1;
                    for(int x:initial_value.back()){
                        if(fixed[-x]>maxi_t){
                            maxi_t = fixed[-x];
                        }
                    }
/*cout << "NEW CLAUSE ";
for(int i:initial_value.back())
cout << i << " ";
cout << endl;*/
//cout << "UID " << *(litConflict.begin()) << endl;
                    if(bInterac){
                        pause(edges,*(litConflict.begin()));
                    }
                    //currentlit
                    if(bwl){
            //cout << "BACK JUSQUE " << maxi_t << endl;
                        while(!currentLvlCl.empty() && currentLvlCl.back().second == t){
                            activeClauses.insert(currentLvlCl.back().first);
                            currentLvlCl.pop_back();
                        }
                        currentLvlLit.pop_back();
                        while(!currentLvlLit.empty() && fixed[currentLvlLit.back().first]==0){
            //cout << "ANNULE  " << currentLvlLit.back().first << " " << fixed[currentLvlLit.back().first] << endl;
                            currentLvlLit.pop_back();
                        }
                        --t;
                        while(t!=maxi_t){
                            while(!currentLvlCl.empty() && currentLvlCl.back().second == t){
                                activeClauses.insert(currentLvlCl.back().first);
                                currentLvlCl.pop_back();
                            }
                            while(!currentLvlLit.empty() && fixed[currentLvlLit.back().first]==t){
            //cout << "ANNULE " << currentLvlLit.back().first << " " << fixed[currentLvlLit.back().first] << endl;
                                fixed[currentLvlLit.back().first]=0;
                                currentLvlLit.pop_back();
                            }
                            --t;
                        }
					}
					else{
                        --t;
                        while(t!=maxi_t){
                            res = b.back(value,activeClauses,fixed,&choice,nbApparPos,nbApparNeg);
                            --t;
                        }
                        while(!currentLvlLit.empty() && !fixed[currentLvlLit.back().first]){
                            currentLvlLit.pop_back();
                        }
                    }
					choice = -*(litConflict.begin());
					fixed[choice]=t;
					currentLvlLit.emplace_back(choice,value.size()-1);
//cout << choice << "  FORCE" << endl;

                    res = evol(choice,true,forcedVariables);
                }
                else{
                    if(bwl){
                        while(!currentLvlCl.empty() && currentLvlCl.back().second == t){
                            activeClauses.insert(currentLvlCl.back().first);
                            currentLvlCl.pop_back();
                        }
                        currentLvlLit.pop_back();
                        while(!currentLvlLit.empty() && fixed[currentLvlLit.back().first]==t){
//cout << currentLvlLit.back().first << " ANNULE" << endl;
                            fixed[currentLvlLit.back().first]=0;
                            choice=-currentLvlLit.back().first;
                            currentLvlLit.pop_back();
                        }
					}
					else
                        choice = -b.lastBack;
//cout << "lastback : " << b.lastBack << endl;
                    --t;
                    fixed[choice]=t;
                    currentLvlLit.emplace_back(choice,value.size()-1);
//cout << choice << "  FORCE" << endl;
                    res = evol(choice,true,forcedVariables);
                }
            }
        }
    }
/*for(auto& x:initial_value){
for(int i:x)
cout << i << " ";
cout << 0 << endl;
}*/
    if(res==1){
        cout << "s SATISFIABLE" << endl;
        for(int i=1;i<=maxVar;++i){
            if(fixed[-i])
                cout << -i << " ";
            else
                cout << i << " ";
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
	unordered_set<int>::iterator pos;
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
int Formule::preTrait(queue<int>& forcedVariables){
	int res;
    set<int> activeClausesCopy(activeClauses);
    for (int i:activeClausesCopy){
        if (value[i].size() == 1){
            int v = *value[i].begin();
            forcedVariables.push(v);
            fixed[v]=t;
            currentLvlLit.emplace_back(v,i);
        } else {
            for (int v:value[i]){
                if (value[i].find(-v) != value[i].end()){
                    activeClauses.erase(i);
                    for(int l:value[i]){
                        reduceAppar(l);
                    }
                    break;
                }
            }
        }
    }
    /*while(!forcedVariables.empty()){
        int choice = forcedVariables.front();
        forcedVariables.pop();
        res = evol(choice, true, forcedVariables);
    }*/
	return activeClauses.empty();
}

//return values :
//0:continue, 1:succeed, 2:fail, -1 backtrack
int Formule::polUnique(queue<int>& forcedVariables){
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
void Formule::graphe(vector<pair<int,int>>& edges, int uid){
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

void Formule::pause(vector<pair<int,int>>& edges, int uid){
	cout << "g : dessiner le graphe des conflits" << endl;
	cout << "c : continuer jusqu'au prochain conflit" << endl;
	cout << "t : finir le déroulement de dpll sans intéruption" << endl;
	char rep;
	while(true){
		scanf(" %c", &rep);
		switch(rep){
			case 'g':
				graphe(edges,uid);
				cout << "c : continuer jusqu'au prochain conflit" << endl;
				cout << "t : finir le déroulement de dpll sans intéruption" << endl;
				while(true){
					scanf(" %c",&rep);
					switch(rep){
						case 'c':
							return;
						case 't':
							bInterac=false;
							return;
						default:
							cout << "Réponse incorrecte." << endl;
							break;
					}
				}
			case 'c':
				return;
			case 't':
				bInterac=false;
				return;
			default:
				cout << "Réponse incorrecte." << endl;
				break;
		}
	}
}



































