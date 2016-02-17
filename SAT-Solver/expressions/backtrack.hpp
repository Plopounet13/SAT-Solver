#ifndef backtrack_hpp
#define backtrack_hpp

#include <vector>
#include <set>
#include <stack>

class Elem_backtrack{
public:
	Elem_backtrack(int var, int)
	virtual void annule(vector<set<Expr&> >& value, set<int>& activeClauses);
private:
	vector<int> var;
	vector<int> bool val;
	int etape; //1ere affectation (0) ou 2eme (1)
	set<int> clauses_sup; //Set des clauses
	set<int> clauses_ret;
};

class Backtrack{
public:
	virtual void back(vector<set<Expr&> >& value, set<int>& activeClauses);
private:
	stack<elem_backtrack> pile;
};
#endif
