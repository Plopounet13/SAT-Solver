#ifndef formule_hpp
#define formule_hpp

#include <vector>
#include <set>
#include <stack>


class Formule
{
public:
	Formule(Expr& e);
	virtual void dpll(string nomFichier);
	virtual void evolClause(int clause, int var);
	virtual void evol(int var);
private:
	vector<set<Expr&> >& value;
	set<int>& activeClauses;
};


#endif