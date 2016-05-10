#ifndef ALOGORITHM_H
#define ALOGORITHM_H

#define ALG_TPARAMS bool logFlag, class Node_ = SLB_NODE, \
        template <class, class> class GoalHandler = SLB_GOAL_HANDLER, \
        class Heuristic = SLB_HEURISTIC

#define ALG_TARGS logFlag, Node_, GoalHandler, Heuristic

#define ALG_TYPES                                                              \
    using Node = Node_;                                                        \
    using NodeData = typename Node::NodeData;                                  \
    using CostType = typename Node::CostType;                                  \
    using NodeUniquePtr = typename Node::NodeUniquePtr;                        \
    using State = typename Node::State;                                        \
    using MyInstance = Instance<State>;                                        \
    using Neighbor = typename State::Neighbor;                                 \
    using MyAlgorithmLog =                                                     \
        typename std::conditional<logFlag, AlgorithmLog<Node>, Nothing>::type;

#define ALG_DATA                                                               \
    using Base = Algorithm<ALG_TARGS>;                                         \
    using Base::log_;                                                          \
    using Base::instance_;                                                     \
    using Base::start_;                                                        \
    using Base::goalHandler_;                                                  \
    using Base::heuristic_;                                                    \
    using Base::expanded_;                                                     \
    using Base::generated_;                                                    \
    using Base::time_;                                                         \
    using Base::cost_;

/// Common base for algorithms.
/// See \ref http://stackoverflow.com/q/34130672/2725810 for the discussion of
/// the use of template template parameters.
template <ALG_TPARAMS>
struct Algorithm {
    ALG_TYPES

    Algorithm(MyInstance &instance)
        : instance_(instance), start_(instance_.start()),
          goalHandler_(instance_, log_), heuristic_(instance_) {}

    MeasureSet measures() const {
        return {time_, cost_, expanded_, generated_};
    }

    const MyAlgorithmLog &log() { return log_; }

protected:
    MyInstance instance_;
    MyAlgorithmLog log_;
    State start_;
    GoalHandler<State, MyAlgorithmLog> goalHandler_;
    Heuristic heuristic_;

    // Stats
    Measure expanded_{"Expanded"};
    Measure generated_{"Generated"};
    Timer time_{"Time (ms.)"};
    Measure cost_{"Cost"};
};

#endif
