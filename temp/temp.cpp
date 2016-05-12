#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cassert>
#include <functional>
#include <algorithm>
#include <chrono>
#include <type_traits>
#include <utility>
#include "PrettyPrint.h"

// http://stackoverflow.com/a/34209403/2725810
// http://stackoverflow.com/a/28283030/3953764

template <typename...>
struct voider { using type = void; };

template <typename... Ts>
using void_t = typename voider<Ts...>::type;

template <typename C, typename = void_t<>>
struct has_t : std::false_type {};

// template <typename C>
// struct has_t<C, void_t<typename C::T>> : std::true_type {};

template <typename C>
struct has_t<C, void_t<decltype(std::declval<C>().g(std::declval<int &>(),
                                                    std::declval<int &>()))>>
    : std::true_type {};

template <typename C>
auto f(C &) -> typename std::enable_if<has_t<C>::value>::type {
    std::cout << "Has it!" << std::endl;
}

template <typename C>
auto f(C &) -> typename std::enable_if<!has_t<C>::value>::type {
    std::cout << "Doesn't have it!" << std::endl;
}

struct A {
    using T = int;
    A(const std::string &) {}
    void g(int &, int &) const;
};

int main() {
    A a("abc");
    f(a);
    return 0;
}
