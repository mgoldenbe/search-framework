#include <iostream>
#include <memory>
#include <set>
#include <unordered_set>
#include <vector>
//#include <pair>
#include <algorithm>
#include <iomanip>
#include <sstream>

template <class MyAlg>
struct Gen {
    bool x = MyAlg::flag; // this line is fine
    typename MyAlg::T y;  // this does not compile
};

template <class Concrete, bool flag_, template <class> class MyGen>
struct Alg {
    static const bool flag = flag_;
    using T = int;

    MyGen<Concrete> g_; // If we have Alg instead of Concrete here, all works
};

template <bool flag_, template <class> class MyGen = Gen>
struct Astar : Alg<Astar<flag_, MyGen>, flag_, MyGen> {
    using Base = Alg<Astar<flag_, MyGen>, flag_, MyGen>;
    using Base::T;
};

int main() {
    Astar<true> a; (void)a;
    return 0;
}
