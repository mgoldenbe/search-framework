#include <iostream>
#include <memory>
#include <vector>
#include <set>
#include <unordered_set>
//#include <pair>
#include <algorithm>
#include <sstream>
#include <iomanip>

template<class V, typename T>
bool in(const V &v, const T &el) {
    return std::find(v.begin(), v.end(), el) != v.end();
}

struct MyState {
    MyState(int xx) : x(xx) {}
    bool operator==(const MyState &rhs) const {
        return x == rhs.x;
    }
    int x;
};

template <class State>
bool operator==(const std::shared_ptr<const State> &lhs,
                const std::shared_ptr<const State> &rhs) {
    return *lhs == *rhs;
}

int main() {
    std::vector<std::shared_ptr<const MyState>> v{
        std::make_shared<const MyState>(5)};
    auto p = std::make_shared<const MyState>(5);
    std::cout << in(v, p) << std::endl;
    return 0;
}
