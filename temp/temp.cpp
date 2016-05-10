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

template <bool flag> typename std::conditional<flag, int, double>::type f() {
    return 0;
}

void g(int) {}

int main() {
    g(f<true>());
    return 0;
}
