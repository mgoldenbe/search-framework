#ifndef PER_GOAL_H
#define PER_GOAL_H

// Per-Goal is a high-level algorithm that runs some other algorithm for each
// goal and combined results
template <class Algorithm = RAW_ALGORITHM, class Instance = INSTANCE,
          class Graph = GRAPH, class Logger = LOGGER>
struct PerGoal {
    using CostType = typename Algorithm::CostType;
    using SingleGoalInstance = typename Instance::SingleGoalInstance;
    using RawAlgorithm = typename Algorithm::template InstanceTemplate<SingleGoalInstance>;
    PerGoal(Instance &instance, Graph &graph, Logger &logger) :
        instance_(instance), graph_(graph), logger_(logger) {}

    CostType run() {
        CostType res = 0.0;
        for (auto i : instance_.goalInstances()) {
            RawAlgorithm alg(i, graph_, logger_);
            res += alg.run();
            stats_.append(alg.measures());
        }
        return static_cast<double>(res) / instance_.goalInstances().size();
    }

    MeasureSet measures() const {
        return stats_.sum(true)[0];
    }
private:
    Stats stats_;
    const Instance &instance_;
    Graph &graph_;
    Logger &logger_;
};

#endif
