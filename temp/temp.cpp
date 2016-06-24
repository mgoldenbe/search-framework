#include "iostream"

// Using CRTP to enable correct policy types in the abstract algorithm class.
// https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern

// See for explanation:
// http://stackoverflow.com/a/35436437/2725810
// http://stackoverflow.com/a/5534818/2725810

template <typename SubClass> class AlgorithmTraits {};

template <class MyAlgorithm>
struct GoalHandler {
    using T = typename AlgorithmTraits<MyAlgorithm>::T;
    GoalHandler(MyAlgorithm &alg) : alg_(alg) {}
    void f() {
        std::cout << alg_.x << std::endl; // Accessing
    }
    MyAlgorithm &alg_;
};

template <template <class> class GoalHandler>
struct BaseTraits {
    using T = int;
};

template <class Concrete, template <class> class GoalHandler>
struct Algorithm {
    using MyBaseTraits = BaseTraits<GoalHandler>;
    using T = typename MyBaseTraits::T;
    Algorithm() : goalHandler_(static_cast<Concrete &>(*this)) {}
    GoalHandler<Concrete> goalHandler_;
};

template <template <class> class GoalHandler>
struct Astar;   // Forward declare

template <template <class> class GoalHandler>
struct AlgorithmTraits<Astar<GoalHandler>> : BaseTraits<GoalHandler> {};

template <template <class> class GoalHandler>
struct Astar: Algorithm<Astar<GoalHandler>, GoalHandler> {
    using Algorithm<Astar, GoalHandler>::Algorithm;
    using MyType = Astar;
    using Base = Algorithm<MyType, GoalHandler>;
    using T = typename Base::T;//typename Base::T;
    int x = 5;
    void run() { this->goalHandler_.f(); }
};

int main() {
    Astar<GoalHandler> alg; (void)alg;
    alg.run();
}
