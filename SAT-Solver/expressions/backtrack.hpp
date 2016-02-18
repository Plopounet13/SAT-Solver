#ifndef backtrack_hpp
#define backtrack_hpp

#include <vector>
#include <set>
#include <stack>

//Backtrack sur full forced donne fail

class ElemBacktrack{
public:
	ElemBacktrack(int vr, bool vl, bool forc, set<int>& clsup, set<int>& clret);
	virtual bool isForced();
	virtual void annule(vector<set<reference_wrapper<Expr> > >& value, set<int>& activeClauses);
	virtual bool revert(vector<set<reference_wrapper<Expr>>>& value, set<int>& activeClauses);
private:
	int var;			   //etiq
	bool val;			   //Value set for the variable
	bool forced;		   //True -> Variable déduite
	set<int>* clauses_sup; //Set des clauses validées
	set<int>* clauses_ret; //Set des clauses modifiées
};


class Backtrack{
public:
	virtual void push(int vr, bool vl, bool forc, set<int>& clsup, set<int>& clret);
	virtual bool back(vector<set<reference_wrapper<Expr> > >& value, set<int>& activeClauses);
private:
	stack<reference_wrapper<elem_backtrack> >& pile;
};
#endif
