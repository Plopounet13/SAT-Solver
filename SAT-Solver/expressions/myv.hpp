#ifndef MYV_HPP_INCLUDED
#define MYV_HPP_INCLUDED

#include <vector>
using namespace std;
template< typename T >
class myv : public vector<T>
{
public:
    myv<T>(int s,int os);
    T &operator[](int i);
private:
    int offset;
};


#endif // MYV_HPP_INCLUDED
