#include <vector>
#include <iostream>
#include <thread>
#include "union_find.hpp"
#include "equality.hpp"

using namespace std;

class bar {
public:
	void foo(int* a) {
		cout << ++a << endl;
	}
};

int main()
{
	int a=0;
	cout << a << endl;
	std::thread t(&bar::foo, bar(), &a);
	t.join();
	cout << a << endl;
}
/*
int main(){
	vector<Equality> (v);
	v.emplace_back(3,1,false);
	v.emplace_back(1,2,true);
	v.emplace_back(3,4,true);
	v.emplace_back(4,2,true);
	cout << answer(v, 6) <<endl;
}*/