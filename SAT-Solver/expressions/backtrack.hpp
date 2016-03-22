#ifndef backtrack_hpp
#define backtrack_hpp

#include <vector>
#include <set>
#include <stack>
#include <functional>
#include "expr.hpp"

using namespace std;

//Backtrack sur full forced donne fail

class ElemBacktrack{
public:
	ElemBacktrack(int vr, bool forc, set<int>* clsup, set<int>* clret);
	~ElemBacktrack();
	virtual bool isForced();
	virtual void annule(vector<set<int>>* value, set<int>* activeClauses, map<int,bool>* fixed,vector<int>* nbApparPos,vector<int>* nbApparNeg);
	virtual void revert(vector<set<int>>* value, set<int>* activeClauses, map<int,bool>* fixed, int* vr);
	virtual int variable();
private:
	int var;			   //etiq
	bool forced;		   //True -> Variable déduite
	set<int>* clauses_sup; //Set des clauses validées
	set<int>* clauses_ret; //Set des clauses modifiées
};


class Backtrack{
public:
	Backtrack();
	virtual void push(int vr, bool forc, set<int>* clsup, set<int>* clret);
	virtual bool back(vector<set<int>>* value, set<int>* activeClauses, map<int,bool>* fixed, int* var,vector<int>* nbApparPos,vector<int>* nbApparNeg);
	virtual void variables(set<int>& s);
private:
	stack<reference_wrapper<ElemBacktrack> >* pile;
};
#endif
