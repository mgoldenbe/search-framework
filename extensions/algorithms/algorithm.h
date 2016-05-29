#ifndef ALOGORITHM_H
#define ALOGORITHM_H

/// \file
/// \brief The \ref Algorithm class.
/// \author Meir Goldenberg

/// Common template parameters for search algorithms.
// See http://stackoverflow.com/q/34130672/2725810 for the discussion of
// the use of template template parameters.
#define ALG_TPARAMS bool logFlag, class Node_ = SLB_NODE, \
        template <class, class> class GoalHandler = SLB_GOAL_HANDLER, \
        class Heuristic = SLB_HEURISTIC

/// Template arguments to be used for inheriting from \ref Algorithm.
#define ALG_TARGS logFlag, Node_, GoalHandler, Heuristic

/// Type aliases to be defined by all search algorithms.
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

/// Data members of \ref Algorithm to be used by all search algorithms.
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

/// Common base for search algorithms.
template <ALG_TPARAMS>
struct Algorithm {
    ALG_TYPES

    /// Initializes the algorithm based on the problem instance.
    /// \param instance The problem instance.
    Algorithm(MyInstance &instance)
        : instance_(instance), start_(instance_.start()),
          goalHandler_(instance_, log_), heuristic_(instance_) {}

    /// Returns the statistics about the search algorithm's
    /// performance for solving the particular instance.
    /// \return The statistics about the search algorithm's
    /// performance for solving the particular instance.
    MeasureSet measures() const {
        return {time_, cost_, expanded_, generated_};
    }

    /// Returns the log of events generated by the search algorithm.
    /// \return Const reference to the log of events generated by the search
    /// algorithm.
    const MyAlgorithmLog &log() { return log_; }

protected:
    MyInstance instance_; ///< The problem instance.

    /// The log of events generated by the search algorithm.
    MyAlgorithmLog log_;

    /// The start state (multiple start states are to be handled in the future).
    State start_;

    /// The policy for handling conditions related to goal states.
    GoalHandler<State, MyAlgorithmLog> goalHandler_;

    /// The heuristic used by the search algorithm.
    Heuristic heuristic_;

    /// \name Statistics about the search algorithm's performance.
    /// \note These statistics pertain to solving the particular instance.
    /// @{

    /// The number of nodes expanded by the search algorithm.
    Measure expanded_{"Expanded"};

    /// The number of nodes generated by the search algorithm.
    Measure generated_{"Generated"};

    /// Time taken by the search algorithm.
    Timer time_{"Time (ms.)"};

    /// The cost of the solution.
    Measure cost_{"Cost"};
    /// @}
};

#endif
