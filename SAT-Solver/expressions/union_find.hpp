#ifndef union_find_hpp
#define union_find_hpp

#include <iostream>
#include <vector>
#include <unordered_set>
#include "equality.hpp"

using namespace std;

class Union_find{
public:
	Union_find(int maxVar);
	int find(int v);
	bool Union(int x,int y, int step);
	bool unequality(int x, int y, int step);
	bool insert(Equality& e);
private:
	vector<int> tab;
	vector<int> rep;
	vector<vector<bool>> ineq;
};

bool answer(vector<Equality>& e, int maxVar);
#endif /* union_find_hpp */
