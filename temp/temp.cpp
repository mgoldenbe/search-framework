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

template <typename T>
struct A {
    using MyType1 = ...;
    using MyType2 = ...;
    using MyType3 = ...;
    using MyType4 = ...;
    using MyType5 = ...;
    ...
};

template <typename T>
struct B: A<T> {
    using MyType1 = A<T>::MyType1;
    using MyType2 = A<T>::MyType2;
    using MyType3 = A<T>::MyType3;
    using MyType4 = A<T>::MyType4;
    using MyType5 = A<T>::MyType5;
    ...
};

template <typename T>
struct C: A<T> {
    using MyType1 = A<T>::MyType1;
    using MyType2 = A<T>::MyType2;
    using MyType3 = A<T>::MyType3;
    using MyType4 = A<T>::MyType4;
    using MyType5 = A<T>::MyType5;
    ...
};

int main() {
    return 0;
}
