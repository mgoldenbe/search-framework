#include <iostream>
#include <memory>
#include <set>
#include <unordered_set>
#include <vector>
//#include <pair>
#include <algorithm>
#include <iomanip>
#include <sstream>

template <class> struct A;
using B = A<int>;
template <class> struct A { using T = int; };

int main() {
    A<int>::T x; (void)x;
    B::T y; (void)y;
    return 0;
}
