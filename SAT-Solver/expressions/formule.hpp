#ifndef formule_hpp
#define formule_hpp

#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
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
#include <thread>
#include <mutex>
#include "backtrack.hpp"
#include "expr.hpp"
#include "myv.hpp"

#define STANDARD 0
#define RAND 1
#define MOMS 2
#define DLIS 3
#define VSIDS 4

#define FOR_SCORE 10
#define INC_SCORE 64

using namespace std;


class Formule
{
public:
	Formule(Expr& e, int heur);
	Formule(vector<unordered_set<int>>& val, int heur);
	virtual void dpll(string fout);
	virtual int evol(int var, bool forced, queue<int>& forcedVariables);
	virtual int evolWL(bool forced, queue<int>& forcedVariables);
	virtual int choose();
	virtual int chooseWL();
	virtual int propage(int var);
	virtual int preTrait(queue<int>& forcedVariables);
	virtual int polUnique(queue<int>& forcedVariables);
	virtual void pause(vector<pair<int,int>>& edges, int uid);
	virtual void graphe(vector<pair<int,int>>& edges, int uid);
	virtual void reset();
private:
	void retire(int v);
	void reduceAppar(int i);
	int heuristique;
	int nbClauseInit;
	map<int,int> scoreVsids;
	unordered_map<int,int> scoreForget;
    myv<int> fixed;
	myv<mutex> lockFixed;
	vector<unordered_set<int>> value;
	vector<unordered_set<int>> initial_value;
	set<int> activeClauses;
	mutex lockActiveClauses;
	Backtrack b;
	vector<int> nbApparPos;
	vector<int> nbApparNeg;
	vector<mutex> lockNbAppar;
	map<int,set<int> > appar;
	map<int,int> scroreForget;
	vector<vector<int> > valueWL;
	vector<vector<int>::iterator> watched1;
	vector<vector<int>::reverse_iterator> watched2;
	vector<vector<int>> nbApparNegInit;
    vector<vector<int>> nbApparPosInit;
	vector<vector<vector<int>::iterator>> watched1Init;
	vector<vector<vector<int>::reverse_iterator>> watched2Init;
};


#endif
