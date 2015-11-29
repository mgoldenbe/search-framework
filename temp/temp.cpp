#include "type.hpp"
#include <iostream>
//#include <vector>

template <typename T_>
struct A {
    using T=T_;
};

template <typename T_>
struct B: public A<T_> {
    typename A<T_>::T a;
};

int main() {
    B<int>::T b;
    (void)b;
    //std::cout << demangle(typeid(A<decltype(x)>).name()) << std::endl;
    return 0;
}
