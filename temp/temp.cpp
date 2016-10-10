#include <iostream>
#include <memory>
#include <set>
#include <unordered_set>
#include <vector>
//#include <pair>
#include <algorithm>
#include <iomanip>
#include <sstream>

template <class State>
struct has_layout<State,
                  void_t<decltype(std::declval<State>().visualLocation(
                      std::declval<double &>(), std::declval<double &>()))>>
    : std::true_type {};

int main() {
    return 0;
}
