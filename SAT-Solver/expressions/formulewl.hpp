#ifndef formulewl_hpp
#define formulewl_hpp

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
#include "expr.hpp"

#define STANDARD 0
#define RAND 1
#define MOMS 2
#define DLIS 3
#define VSIDS 4

using namespace std;

class Formulewl
{
public:
	Formulewl(Expr& e, int heur);
	Formulewl(vector<set<int>>& val, int heur);
	virtual void dpll(string fout);
	virtual int evol(int var);
	virtual int choose();
	virtual int propage(int var);
	virtual int preTrait();
	virtual int polUnique();
	virtual void pause(vector<pair<int,int>>& edges, int uid);
	virtual void graphe(vector<pair<int,int>>& edges, int uid);
	virtual void backtrack(vector<int> toDel);
private:
	int heuristique;
	map<int,int> scoreVsids;
    map<int,int> fixed;
	vector<vector<int> > value;
	set<int> activeClauses;
	map<int,int> watched1;
	map<int,int> watched2;
	vector<int> nbApparPos;
	vector<int> nbApparNeg;
	map<int,set<int> > appar;
};


#endif
