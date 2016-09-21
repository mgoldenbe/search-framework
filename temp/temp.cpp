#include <iostream>
#include <memory>
#include <vector>
#include <set>
#include <unordered_set>
//#include <pair>
#include <algorithm>
#include <sstream>
#include <iomanip>

template <typename Alg>
struct Generator;

template <typename Alg> struct Heuristic {
    using Neighbor = typename Generator<Alg>::Neighbor;
};

template <typename Alg> struct Generator {
    using Neighbor = int;
    Heuristic<Alg> h;
};

int main()
{
    Heuristic<int> x;    // Version 1 - compile error
    //Generator<int> x;  // Version 2 - compile fine
    (void)x;
}


// http://stackoverflow.com/questions/8401827/crtp-and-type-visibility

/*
template <typename Alg> class AlgorithmTraits;

template <class Alg>
struct Heuristic {
    using Neighbor = typename AlgorithmTraits<Alg>::Generator::Neighbor;
};

template <class Alg, template <class> class HType>
struct Generator {
    using Neighbor = int;
    HType<Alg> h_;
};
template <class Alg>
using GeneratorPolicy = Generator<Alg, Heuristic>;

template <template <class> class InitialHeuristic_,
          template <class> class Generator_> class Astar;
template <template <class> class InitialHeuristic_,
          template <class> class Generator_>
struct AlgorithmTraits<Astar<InitialHeuristic_, Generator_>> {
    using MyAlgorithm = Astar<InitialHeuristic_, Generator_>;
    using InitialHeuristic = InitialHeuristic_<MyAlgorithm>;
    using Generator = Generator_<MyAlgorithm>;
};

template <template <class> class InitialHeuristic_,
          template <class> class Generator_>
class Astar {
    using InitialHeuristic = typename AlgorithmTraits<Astar>::InitialHeuristic;
    using Generator = typename AlgorithmTraits<Astar>::Generator;
    InitialHeuristic h_; // version 1 (does not compile)
    //Generator g_;          // version 2 (compiles)
};

int main() {
    Astar<Heuristic, GeneratorPolicy> a; (void)a;
    return 0;
}
*/
