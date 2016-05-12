#include "union_find.hpp"

using namespace std;

Union_find::Union_find(int maxVar):rep(maxVar+1), tab(maxVar+1), ineq(maxVar+1,vector<bool>(maxVar+1)){
	for (int i=0; i<tab.size(); ++i){
		rep[i]=i;
	}
}

int Union_find::find(int v){
	if (rep[v] != v)
		rep[v]=find(rep[v]);
	return rep[v];
}

bool Union_find::Union(int x, int y, int step){
	int repx=find(x);
	int repy=find(y);
	if (repx==repy){
		return true;
	}
	if(ineq[rep[x]][rep[y]])
		return false;
	if (tab[repx] < tab[repy])
		rep[repx]=repy;
	else if (tab[repx] < tab[repy])
		rep[repy]=repx;
	else{
		rep[repy]=repx;
		tab[repx] += 1;
	}
	for (int i=0; i<ineq.size(); ++i){
		ineq[rep[repx]][i]=ineq[repx][i]||ineq[repy][i];
		ineq[i][rep[repx]]=ineq[i][repx]||ineq[i][repy];
	}
	return true;
}

bool Union_find::unequality(int x, int y, int step){
	if (rep[x]==rep[y])
		return false;
	ineq[rep[x]][rep[y]]=true;
	ineq[rep[y]][rep[x]]=true;
	return true;
}

bool Union_find::insert(Equality& e){
	if (e.x > tab.size() || e.y > tab.size()){
		cerr << "variable hors borne equality" << endl;
		return false;
	}
	if (e.isEquality)
		return Union(e.x, e.y, 1);
	else
		return unequality(e.x, e.y, 1);
	
}

bool answer(vector<Equality>& v, int maxVar){
	Union_find u(maxVar);
	for (auto& e:v){
		if (!u.insert(e))
			return false;
	}
	return true;
}