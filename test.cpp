/// \file
/// \brief The standard main file, which uses the framework for the most common
/// experimentation scenarios -- running a search algorithm for a set of problem
/// instances, running a search algorithm for a single problem instance with
/// visualization and creating a new set of problem instances.
/// \author Meir Goldenberg

// Can be pre-compiled (~25% compile-time reduction)
#include "outside_headers.h"

#ifdef CONFIG
#include CONFIG
#else
#include "projects/KGoal/ConfigMinHeuristic.h"
// //#include "projects/KGoal/ConfigPerGoal.h"
// //#include "projects/KGoal/ConfigUniformSearch.h"
#endif

#include "core/headers.h"
#include "extensions/headers.h"

template <class MyAlgorithm>
using buildGraphOL =
    OpenList_T<MyAlgorithm, SLB_NODE, DefaultOLKeyType, GreaterPriority_SmallG>;

/// Builds the state graph of the domain.
/// \return The state graph of the domain.
StateGraph<SLB_STATE> buildGraph() {
    using MyInstance = Instance<SLB_STATE>;

    auto instance =
        MyInstance(std::vector<SLB_STATE>(1), std::vector<SLB_STATE>(1));
    Astar<false, SLB_NODE, NoGoalHandler, ZeroHeuristic, buildGraphOL> myAstar(
        instance);
    myAstar.run();
    return myAstar.graph();
}

/// Runs a search algorithm for a set of problem instances.
void run() {
    if (CMD.spaceInitFileName_isSet())
        SLB_STATE::initSpace(CMD.spaceInitFileName());

    auto res = readInstancesFile<SLB_STATE>(CMD.instancesFileName());
    Stats stats;
    if (CMD.visualizeInstance() >= 0) {
        if (CMD.visualizeInstance() >= static_cast<int>(res.size()))
            throw std::invalid_argument(
                "Can't visualize the requested instance as there are only " +
                str(res.size()) + " instances.");
        SLB_ALGORITHM<true> alg(res[CMD.visualizeInstance()]);
        alg.run();
        stats.append(alg.measures(), CMD.perInstance());
        auto g = buildGraph();
        //g.dump();
        Visualizer<SLB_NODE> vis(g, alg.log());
        vis.run();
    } else {
        for (auto instance : res) {
            SLB_ALGORITHM<false> alg(instance);
            alg.run();
            auto instanceMeasures = instance.measures();
            stats.append(instanceMeasures.append(alg.measures()),
                         CMD.perInstance());
        }
    }
    if (!CMD.perInstance())
        stats = stats.average();
    Table statsTable;
    stats.dump(statsTable, CMD.prefixTitle(), CMD.prefixData());
    if (CMD.hideTitle()) statsTable.hideTitle();
    std::cout << statsTable;
}

/// Creates a new set of problem instances and saves it in a file.
void makeInstances() {
    if (CMD.spaceInitFileName_isSet())
        SLB_STATE::initSpace(CMD.spaceInitFileName());
    makeInstancesFile<SLB_STATE>(CMD.instancesFileName());
}

/// Reads the command line and dispatches to run one of the scenarios.
/// \param argc Number of command line arguments.
/// \param argv The command line arguments.
/// \return The exit code.
int main(int argc, char **argv) {
    try {
        CommandLine::instance(argc, argv); // to initialize command line
        if (CMD.nInstances() != -1)
            makeInstances();
        else
            run();
    }
    catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
