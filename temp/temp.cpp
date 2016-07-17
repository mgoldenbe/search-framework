#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
//#include <pair>
#include <algorithm>

struct A {
    A(int x) : x_(x) {}
private:
    int x_;
};

template <class Mixin>
struct B: Mixin {
    B() : x_(5), Mixin(5) {}
private:
    int x_;
};

int main() {
    B<A> b; (void)b;
    return 0;
}
