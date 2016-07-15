#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
//#include <pair>
#include <algorithm>

struct A {
    int x = 5;
};

int main() {
    A a;
    std::vector<std::pair<A, int>> v;
    v.push_back(std::make_pair(a, 1));
    std::cout << v[0].first.x << std::endl;
    return 0;
}
