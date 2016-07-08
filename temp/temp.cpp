#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include <algorithm>

/*
template <class T = C>
struct A {
    // ... definitions that require that C be complete
};

template <class T = A>
struct B {
    // ... definitions that require that A be complete
};

template <class T = B>
struct C {
    // ... definitions that require that B be complete
};
*/

template <class T>
struct A_t {
    // ...
};

template <class T>
struct B_t {
    // ...
};

template <class T>
struct C_t {
    //
};

template <class T = C_t>
using A = A_t<T>;

template <class T = A_t>
using B = B_t<T>;

template <class T = B_t>
using C = C_t<T>;

/*
template <class T>
struct A_t {
    void f() {
      std::cout << "Here" << std::endl;
    }
};

class DefaultT{};

template <class T = DefaultT>
using A = A_t<T>;
*/

int main() {
    A<>{} a; (void)a;
    return 0;
}
