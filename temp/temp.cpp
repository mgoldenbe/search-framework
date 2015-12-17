#include <algorithm>
#include <iostream>
#include <vector>
#include "ManagedNode.h"

struct A : public ManagedNode<> {
    REFLECTABLE((int)f)
};

struct B : public ManagedNode<A> {};

struct C : public ManagedNode<B> {};

int main() {
    C c; (void)c;
    std::cout << c << std::endl;
}
