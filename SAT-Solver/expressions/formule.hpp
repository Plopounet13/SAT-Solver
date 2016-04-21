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
#include <climits>
#include <cstdlib>
#include <string>
#include <fstream>
#include "backtrack.hpp"
#include "expr.hpp"

#define STANDARD 0
#define RAND 1
#define MOMS 2
#define DLIS 3
#define VSIDS 4
#define FORGET 5

#define INC_SCORE 128

using namespace std;

class Formule
{
public:
	Formule(Expr& e, int heur);
	Formule(vector<set<int>>& val, int heur);
	virtual void dpll(string fout);
	virtual int evol(int var, bool forced, queue<int>& forcedVariables);
	virtual int choose();
	virtual int propage(int var);
	virtual int preTrait(queue<int>& forcedVariables);
	virtual int polUnique(queue<int>& forcedVariables);
	virtual void pause(vector<pair<int,int>>& edges, int uid);
	virtual void graphe(vector<pair<int,int>>& edges, int uid);
private:
	int heuristique;
	int nbClauseInit;
	map<int,int> scoreVsids;
    map<int,int> fixed;
	vector<set<int>> value;
	vector<set<int>> initial_value;
	set<int> activeClauses;
	vector<pair<int,int> > watched;
	Backtrack b;
	vector<int> nbApparPos;
	vector<int> nbApparNeg;
	map<int,set<int> > appar;
};


#endif
