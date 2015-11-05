#include "type.hpp"
#include <iostream>
#include <vector>
#include <boost/functional/hash.hpp>

int main() {
    std::vector<int> v;
    boost::hash<std::vector<int>> v_hash;
    for (int i = 0; i != 10000; i++) {
        v.push_back(1);
        std::size_t h = v_hash(v);
        std::cout << h  << std::endl;
    }

    //std::cout << demangle(typeid(A<decltype(x)>).name()) << std::endl;
    return 0;
}
