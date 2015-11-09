#include "type.hpp"
#include <iostream>
//#include <vector>

int main() {
    int a = 5;
    int &b = a;
    std::cout << &a << " "<< &b << std::endl;

    //std::cout << demangle(typeid(A<decltype(x)>).name()) << std::endl;
    return 0;
}
