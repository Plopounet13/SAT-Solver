#ifndef backtrack_hpp
#define backtrack_hpp

#include <vector>
#include <set>
#include <stack>

//Backtrack sur full forced donne fail

class Elem_backtrack{
public:
	Elem_backtrack(int var, int)
	virtual void annule(vector<set<Expr&> >& value, set<int>& activeClauses);
private:
	int var;
	bool val;
	int forced;
	set<int> clauses_sup; //Set des clauses
	set<int> clauses_ret;
};

class Backtrack{
public:
	virtual void back(vector<set<Expr&> >& value, set<int>& activeClauses);
private:
	stack<elem_backtrack&> pile;
};
#endif
