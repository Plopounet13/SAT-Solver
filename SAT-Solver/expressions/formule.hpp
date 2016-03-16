#ifndef formule_hpp
#define formule_hpp

#include <vector>
#include <set>
#include <stack>
#include <list>
#include <functional>
#include "backtrack.hpp"
#include "expr.hpp"

using namespace std;

class Formule
{
public:
	Formule(Expr& e);
	Formule(vector<set<int>>* val);
	virtual void dpll(string fout);
	virtual int evol(int var, bool val, bool forced);
	virtual pair<int,bool> choose();
	virtual int propage(int var, bool val);
	virtual int preTrait();
private:
    map<int,int>* fixed;
	vector<set<int>>* value;
	set<int>* activeClauses;
	vector<pair<int,int> >* watched;
	Backtrack b;
};


#endif
