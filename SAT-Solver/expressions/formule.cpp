#include <iostream>

#include "formule.hpp"

mutex lockClausesToDel;
mutex lockForcedVariables;
mutex lockio;
extern mutex lockFini;
extern bool bFini;
extern int maxVar;
//bool bParal = false;

void Formule::reset(){
    watched1.swap(watched1Init[t-1]);
    watched1Init.resize(t-1);
    watched2.swap(watched2Init[t-1]);
    watched2Init.resize(t-1);
    nbApparPos.swap(nbApparPosInit[t-1]);
    nbApparPosInit.resize(t-1);
    nbApparNeg.swap(nbApparNegInit[t-1]);
    nbApparNegInit.resize(t-1);
}


Formule::Formule(Expr& e, int heur, bool cl, bool Interac, bool Forget, bool wl):heuristique(heur),fixed(0,0), lockFixed(0,0), bcl(cl), bInterac(Interac), bForget(Forget), bwl(wl){
    value = *toEns(e);
    if(bwl){
        for(auto& c:value){
            valueWL.emplace_back();
            for(int v:c){
                valueWL.back().push_back(v);
            }
        }
    }
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
        watched1.clear();
        watched2.clear();
        for(auto& c:valueWL){
            watched1.push_back(c.begin());
            watched2.push_back(c.rbegin());
        }
    }
}

Formule::Formule(vector<unordered_set<int>> val, int heur, bool cl, bool Interac, bool Forget, bool wl):heuristique(heur),fixed(2*maxVar+1,maxVar),lockFixed(2*maxVar+1,maxVar), bcl(cl), bInterac(Interac), bForget(Forget), bwl(wl){
	value = val;
    if(bwl){
        for(auto& c:value){
            valueWL.emplace_back();
            for(int v:c){
                valueWL.back().push_back(v);
            }
        }
    }
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
        watched1.clear();
        watched2.clear();
        for(auto& c:valueWL){
            watched1.push_back(c.begin());
            watched2.push_back(c.rbegin());
        }
    }
}

void Formule::retireParal(int c){
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

/*void Formule::reduceApparParal(queue<int>& forcedVariables, int i, bool bFixedLocked){
///CA BUG ICI
	lockNbAppar[abs(i)].lock();
    if(i>0)
        --(nbApparPos[i]);
    else
		--(nbApparNeg[-i]);
	lockNbAppar[abs(i)].unlock();
    if(!bFixedLocked){
        lockFixed[-abs(i)].lock();
        lockFixed[abs(i)].lock();
    }
    if(!bcl and nbApparPos[abs(i)]+nbApparNeg[abs(i)]!=0 and !fixed[i] and !fixed[-i]){
        lockCurrentLvlLit.lock();
        if(bFini){
            if(!bFixedLocked){
                lockFixed[-abs(i)].unlock();
                lockFixed[abs(i)].unlock();
            }
            lockCurrentLvlLit.unlock();
            return;
        }
        if(nbApparPos[abs(i)]==0){
            if(!fixed[-abs(i)]){
                fixed[-abs(i)]=t;
                forcedVariables.push(-abs(i));
                currentLvlLit.emplace_back(-abs(i),0);
            }
        }
        if(nbApparNeg[abs(i)]==0){
            if(!fixed[abs(i)]){
                fixed[abs(i)]=t;
                forcedVariables.push(abs(i));
                currentLvlLit.emplace_back(abs(i),0);
            }
        }
        lockCurrentLvlLit.unlock();
	}
    if(!bFixedLocked){
        lockFixed[-abs(i)].unlock();
        lockFixed[abs(i)].unlock();
    }
}*/

void Formule::retire(int c){
	if (activeClauses.erase(c))
		for (auto& v:value[c]){
			if (v<0)
				--(nbApparNeg[-v]);
			else
				--(nbApparPos[v]);
		}
}

void Formule::reduceAppar(queue<int>& forcedVariables, int i){
	if(i>0)
		--(nbApparPos[i]);
	else
		--(nbApparNeg[-i]);
	if(!bcl and nbApparPos[abs(i)]+nbApparNeg[abs(i)]!=0 and !fixed[i] and !fixed[-i]){
		if(nbApparPos[abs(i)]==0){
			if(!fixed[-abs(i)]){
				fixed[-abs(i)]=t;
				forcedVariables.push(-abs(i));
				currentLvlLit.emplace_back(-abs(i),0);
			}
		}
		if(nbApparNeg[abs(i)]==0){
			if(!fixed[abs(i)]){
				fixed[abs(i)]=t;
				forcedVariables.push(abs(i));
				currentLvlLit.emplace_back(abs(i),0);
			}
		}
	}
}

/*void Formule::boucleThread(set<int>::iterator start, set<int>::iterator end, queue<int>* forVar, vector<int>* clToDel, int* rv){
	queue<int>& forcedVariables=*forVar;
	vector<int>& clausesToDel=*clToDel;
	int& retVal=*rv;
	int k = 0;
	for(auto i=start; i!=end; ++i){
		int c=*i;
		while(watched1[c] != watched2[c].base() and fixed[-*watched1[c]]){
			/// à supprimer
			reduceApparParal(forcedVariables, *watched1[c],false);
			++watched1[c];
		}
		/// On vérifie si c est une clause fausse ///
		if(watched1[c] == watched2[c].base()){
			lockCurrentLvlLit.lock();
			if(!bFini){
                currentLvlLit.emplace_back(0,c);
                bFini = true;
                lockCurrentLvlLit.unlock();
            }
            else{
                lockCurrentLvlLit.unlock();
                return;
            }
			if(t!=1){
				retVal = -1;
				return;
			}else{
				retVal = 2;
				return;
			}
		}
		/// Actualisation de watched2[c] ///
		while(fixed[-*watched2[c]]){
			/// à supprimer
			reduceApparParal(forcedVariables, *watched2[c],false);
			++watched2[c];
		}
		/// On vérifie si c est une clause vraie ///
		int w[4] = {*watched1[c],*watched2[c],-*watched1[c],-*watched2[c]};
		sort(w,w+4);
		if(fixed[*watched1[c]] or fixed[*watched2[c]]){
			lockClausesToDel.lock();
			clausesToDel.push_back(c);
			lockClausesToDel.unlock();
			vector<int>::iterator it=watched1[c];
			for(; it!=watched2[c].base(); ++it){
				reduceApparParal(forcedVariables, *it,false);
			}
		}
		/// On vérifie si c est une clause unitaire ///
		else if(watched1[c] == watched2[c].base()-1){
			if (bForget){
				lockScoreForget.lock();
				if(scoreForget.count(c))
					scoreForget[c]+=value.size();
				for (auto& s:scoreForget){
					if (s.first != c)
						--s.second;
					if (s.second <= 0)
						retireParal(s.first);
				}
				lockScoreForget.unlock();
			}
            lockFixed[w[0]].lock();
            lockFixed[w[3]].lock();
            if(!fixed[-*watched1[c]]){
                lockCurrentLvlLit.lock();
                if(bFini){
                    lockCurrentLvlLit.unlock();
                    lockFixed[w[0]].unlock();
                    lockFixed[w[3]].unlock();
                    return;
                }
                currentLvlLit.emplace_back(*watched1[c],c);
                lockCurrentLvlLit.unlock();
                lockForcedVariables.lock();
                forcedVariables.push(*watched1[c]);
                lockForcedVariables.unlock();
                fixed[*watched1[c]]=t;
                //reduceApparParal(forcedVariables, *watched1[c],true);
                lockFixed[w[0]].unlock();
                lockFixed[w[3]].unlock();
            }
            else{
                lockFixed[w[0]].unlock();
                lockFixed[w[3]].unlock();
                if(t!=1){
                    retVal = -1;
                    return;
                }else{
                    retVal = 2;
                    return;
                }

            }
		}
	}
	retVal=0;
	return;
}*/

int Formule::evolWL(bool forced, queue<int>& forcedVariables){
    vector<int> clausesToDel;

	/*if (bParal){
		int nbThreads=thread::hardware_concurrency();
		if (!nbThreads)
			nbThreads=1;
		vector<thread> threads;
		vector<int> returns(nbThreads);
		int pas = activeClauses.size()/nbThreads;
		set<int>::iterator start=activeClauses.begin();
		set<int>::iterator end=activeClauses.end();
		bFini = false;
		cout << "CA PART " << t << endl;
		for (int i=0; i<nbThreads; ++i){
			if (i!=nbThreads-1){
				set<int>::iterator fin = start;
				advance(fin,pas);
				threads.emplace_back(&Formule::boucleThread, this, start, fin, &forcedVariables, &clausesToDel, &(returns[i]));
				start=fin;
			}else
				threads.emplace_back(&Formule::boucleThread, this , start, end, &forcedVariables, &clausesToDel, &(returns[i]));
		}
		int valReturn=0;
		for (int i=0; i<threads.size(); ++i){
			threads[i].join();
			if (returns[i] && !valReturn){
				valReturn=returns[i];
			}
		}
		cout << "FINI "<< t << endl;
		if (valReturn)
			return valReturn;
	}else{*/
		for (int c:activeClauses){
			while(watched1[c] != watched2[c].base() and fixed[-*watched1[c]]){
			/// à supprimer
				reduceAppar(forcedVariables, *watched1[c]);
				++watched1[c];
			}
			if(watched1[c] == watched2[c].base()){
				currentLvlLit.emplace_back(0,c);
				if(t!=1)
					return -1;
				else
					return 2;
			}
			while(fixed[-*watched2[c]]){
				/// à supprimer
				reduceAppar(forcedVariables, *watched2[c]);
				++watched2[c];
			}
			if(fixed[*watched1[c]] or fixed[*watched2[c]]){
				clausesToDel.push_back(c);
				vector<int>::iterator it=watched1[c];
				for(; it!=watched2[c].base(); ++it){
					reduceAppar(forcedVariables, *it);
				}
			}
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
				clausesToDel.push_back(c);
				forcedVariables.push(*watched1[c]);
				fixed[*watched1[c]]=t;
				currentLvlLit.emplace_back(*watched1[c],c);
				reduceAppar(forcedVariables, *watched1[c]);
			}
		}
	//}

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
            reduceAppar(forcedVariables, -var);
			value[c].erase(-var);
			if (value[c].empty()){
                bBack=true;
                cBack = c;
			}
		}
        while(itP!=endP and *itP < c)
            ++itP;
		if(itP!=endP and *itP == c){
            for(int i:value[c]){
                reduceAppar(forcedVariables, i);
            }
            clausesToDel.push_back(c);
            clauses_sup->insert(c);
        }
        if (value[c].size()==1){
            if(!fixed[*value[c].begin()]){
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
            vector<int> nb1(nbVar+1);
            vector<int> nb2(nbVar+1);
            for(int c:activeClauses){
                for(auto it=watched1[c];it != watched2[c].base();++it){
                if(*it>0)
                ++nb1[*it];
                else
                ++nb2[-*it];
                }
            }
			int maxi=0;
			for (int i=0; i<nb1.size(); ++i){
				if (nb1[i]>maxi and !fixed[i] and !fixed[-i]){
					maxi = nb1[i];
					var = i;
				}
            }
			for (int i=0; i<nb2.size(); ++i){
				if (nb2[i]>maxi and !fixed[i] and !fixed[-i]){
					maxi = nb2[i];
					var = -i;
				}
            }
			if (maxi == 0)
				cout << "plop" << endl;
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
            vector<int> nb1(nbVar+1);
            vector<int> nb2(nbVar+1);
            for(int c:activeClauses){
                for(auto it=watched1[c];it != watched2[c].base();++it){
                if(*it>0)
                ++nb1[*it];
                else
                ++nb2[-*it];
                }
            }
			int maxi=0;
			for (int i=0; i<nb1.size(); ++i){
				if (nb1[i]>maxi and !fixed[i] and !fixed[-i]){
					maxi = nb1[i];
					var = i;
				}
            }
			for (int i=0; i<nb2.size(); ++i){
				if (nb2[i]>maxi and !fixed[i] and !fixed[-i]){
					maxi = nb2[i];
					var = -i;
				}
            }
			if (maxi == 0)
				cout << "plop" << endl;
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
    while(res<=0){
        if(bFini){
            return;
        }
        if(res<=0){
            if(forcedVariables.empty()){
                choice = choose();
                ++t;
                if(bwl){
                    nbApparNegInit.push_back(nbApparNeg);
                    nbApparPosInit.push_back(nbApparPos);
                    watched1Init.push_back(watched1);
                    watched2Init.push_back(watched2);
                }
                fixed[choice]=t;
                currentLvlLit.emplace_back(choice,-1);
                res = evol(choice, false, forcedVariables);
            }
            else{
                choice = forcedVariables.front();
                forcedVariables.pop();
                res = evol(choice, true, forcedVariables);
            }
			while(res==-1){
                while(!bwl and !forcedVariables.empty()){
                    fixed[forcedVariables.front()]=0;
                    forcedVariables.pop();
                }
                if(bcl){
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
                                        else if(fixed[-v] and initial_value.back().count(v)==0){
                                            initial_value.back().insert(v);
					if(bwl){
                                            	if(v>0)
                                                	++nbApparPos[v];
                                            	else
                                                	++nbApparNeg[-v];
                                                valueWL.back().push_back(v);
                                                if(v>0)
                                                    for(auto& vec:nbApparPosInit)
                                                        ++vec[v];
                                                else
                                                    for(auto& vec:nbApparNegInit)
                                                        ++vec[-v];
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
                        for(auto& vec:watched1Init)
                            vec.push_back(valueWL.back().begin());
                        for(auto& vec:watched2Init)
                            vec.push_back(valueWL.back().rbegin());
                        if(v>0)
                            for(auto& vec:nbApparPosInit)
                                ++vec[v];
                        else
                            for(auto& vec:nbApparNegInit)
                                ++vec[-v];
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
                    if(bInterac){
                        pause(edges,*(litConflict.begin()));
                    }
                    if(bwl){
                        while(!currentLvlCl.empty() && currentLvlCl.back().second == t){
                            activeClauses.insert(currentLvlCl.back().first);
                            currentLvlCl.pop_back();
                        }
                        currentLvlLit.pop_back();
                        while(!currentLvlLit.empty() && fixed[currentLvlLit.back().first]==0){
                            currentLvlLit.pop_back();
                        }
                        --t;
                        while(t!=maxi_t){
                            while(!currentLvlCl.empty() && currentLvlCl.back().second == t){
                                activeClauses.insert(currentLvlCl.back().first);
                                currentLvlCl.pop_back();
                            }
                            while(!currentLvlLit.empty() && fixed[currentLvlLit.back().first]==t){
                                fixed[currentLvlLit.back().first]=0;
                                currentLvlLit.pop_back();
                            }
                            --t;
                        }
                        reset();
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
                            fixed[currentLvlLit.back().first]=0;
                            choice=-currentLvlLit.back().first;
                            currentLvlLit.pop_back();
                        }
                        --t;
                        reset();
					}
					else{
                        choice = -b.lastBack;
                        --t;
                    }
                    fixed[choice]=t;
                    currentLvlLit.emplace_back(choice,value.size()-1);
                    res = evol(choice,true,forcedVariables);
                }
            }
        }
    }
    lockFini.lock();
    if(bFini){
        lockFini.unlock();
        return;
    }
    else{
        bFini=true;
    }
    lockFini.unlock();
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
                        reduceAppar(forcedVariables, l);
                    }
                    break;
                }
            }
        }
    }
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
