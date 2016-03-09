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
	Formule(vector<set<reference_wrapper<Expr>>>* val);
	virtual void dpll(string fout);
	virtual int evol(int var, bool val, bool forced);
	virtual pair<int,bool> choose();
	virtual int preTrait();
private:
    map<int,int>* fixed;
	vector<set<reference_wrapper<Expr>>>* value;
	set<int>* activeClauses;
	Backtrack b;
};


#endif
