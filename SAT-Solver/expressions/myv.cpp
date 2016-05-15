#include "myv.hpp"

template< typename T >
T& myv<T>::operator[](int i)
{
    return vector<T>::operator[](i+offset);
}

template< typename T >
myv<T>::myv(int s, int os):vector<T>(s),offset(os){}

template class myv<int>;
template class myv<mutex>;
