#include "iostream"

template <class T1, class T2> struct P{};

template <template <class> class T1> struct A {
    using TT1 = T1<int>;
};

int main() {
    A<P<T2=int>> a; (void)a;
    return 0;
}
