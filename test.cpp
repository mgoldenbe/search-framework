///@file
///@brief NEED TO MAKE SURE THAT THE CODE IS AS SUCCINCT AS POSSIBLE.

// Can be pre-compiled (~25% compile-time reduction)
#include "outside_headers.h"

//#include CONFIG
#include "projects/KGoal/ConfigMinHeuristic.h"
//#include "ConfigPerGoal.h"
//#include "ConfigUniformSearch.h"

#include "core/headers.h"
#include "extensions/headers.h"

#define CAT(X, Y) X ## Y
#define CMB(A, B) CAT(A, B)
#define SLB_LOGGING_ALGORITHM CMB(Logging, SLB_ALGORITHM)

/**
 * @brief Builds the domain graph
 *
 *
 * @return The domain graph
 */
StateGraph<SLB_STATE> buildGraph() {
    using MyOL = OpenList<SLB_NODE, DefaultPriority, GreaterPriority_SmallG>;
    using MyHeuristic = ZeroHeuristic<SLB_STATE>;
    using MyInstance = Instance<SLB_STATE>;

    Nothing logger;
    auto instance =
        MyInstance(std::vector<SLB_STATE>(1), std::vector<SLB_STATE>(1));
    Astar<MyOL, NoGoalHandler, MyHeuristic, Nothing> myAstar(instance, logger);
    myAstar.run();
    return myAstar.graph();
}

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
        AlgorithmLog<SLB_NODE> logger; (void)logger;
        SLB_LOGGING_ALGORITHM alg(res[CMD.visualizeInstance()], logger);
        alg.run();
        stats.append(alg.measures(), CMD.perInstance());
        auto g = buildGraph();
        Visualizer<SLB_NODE, false> vis(g, logger);
        vis.run();
    } else {
        for (auto instance : res) {
            Nothing logger; (void)logger;
            SLB_ALGORITHM alg(instance, logger);
            alg.run();
            stats.append(alg.measures(), CMD.perInstance());
        }
    }
    if (!CMD.perInstance())
        stats = stats.average();
    Table statsTable;
    stats.dump(statsTable, CMD.prefixTitle(), CMD.prefixData());
    if (CMD.hideTitle()) statsTable.hideTitle();
    std::cout << statsTable;
}

void makeInstances() {
    if (CMD.spaceInitFileName_isSet())
        SLB_STATE::initSpace(CMD.spaceInitFileName());
    makeInstancesFile<SLB_STATE>(CMD.instancesFileName());
}

int main(int argc, char **argv) {
    // need to catch exceptions
    CommandLine::instance(argc, argv); // to initialize command line
    if (CMD.nInstances() != -1)
        makeInstances();
    else
        run();
    return 0;
}
