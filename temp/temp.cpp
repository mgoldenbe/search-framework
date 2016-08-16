#include <iostream>
#include <memory>
#include <vector>
#include <set>
#include <unordered_set>
//#include <pair>
#include <algorithm>
#include <sstream>
#include <iomanip>

template <typename T>
struct A;
struct B;

template <typename T>
using TT = A<B>;

template <typename T>
struct A {};

int main() {
    return 0;
}
