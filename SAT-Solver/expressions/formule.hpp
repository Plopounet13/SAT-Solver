#ifndef formule_hpp
#define formule_hpp

#include <vector>
#include <set>
#include <stack>
#include <functional>
#include "backtrack.hpp"
#include "expr.hpp"
using namespace std;

class Formule
{
public:
	Formule(Expr& e);
	virtual void dpll(string fout);
	virtual int evol(int var, bool val, bool forced);
	virtual pair<int,bool> choose();
private:
	vector<set<reference_wrapper<Expr> > > value;
	set<int> activeClauses;
	Backtrack b;
};


#endif
