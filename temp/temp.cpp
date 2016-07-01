#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include <algorithm>

struct A {
    void f() {
        x = 5;
    }
    void dump() { std::cerr << x << std::endl;}
    int x = 2;
};

int main() {
    A{}.dump();
    A{}.f();
    return 0;
}
