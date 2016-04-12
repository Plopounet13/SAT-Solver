#ifndef formule_hpp
#define formule_hpp

#include <vector>
#include <set>
#include <stack>
#include <list>
#include <functional>
#include <queue>
#include <cstdlib>
#include <ctime>
#include "backtrack.hpp"
#include "expr.hpp"

#define STANDARD 0
#define RAND 1
#define MOMS 2
#define DLIS 3

using namespace std;

class Formule
{
public:
	Formule(Expr& e, int heur);
	Formule(vector<set<int>>* val, int heur);
	virtual void dpll(string fout);
	virtual int evol(int var, bool forced, queue<int>& forcedVariables);
	virtual int choose();
	virtual int propage(int var);
	virtual int preTrait(queue<int>& forcedVariables);
	virtual int polUnique(queue<int>& forcedVariables);
private:
	int heuristique;
    map<int,bool>* fixed;
	vector<set<int>>* value;
	set<int>* activeClauses;
	vector<pair<int,int> >* watched;
	Backtrack b;
	vector<int>* nbApparPos;
	vector<int>* nbApparNeg;
};


#endif
