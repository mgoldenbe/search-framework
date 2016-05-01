///@file
///@brief NEED TO MAKE SURE THAT THE CODE IS AS SUCCINCT AS POSSIBLE.

// This header needs to be here for pre-compilation to work
// Pre-compilation helps little at this point (25% compile-time reduction):
//     http://stackoverflow.com/q/13309228/2725810
// Things to try in the future:
//     -- http://stackoverflow.com/q/10959009/2725810
//     -- identifying several translation units
//        and compiling in parallel with make -j
#include "outside_headers.h"

//#include CONFIG
#include "projects/KGoal/ConfigMinHeuristic.h"
//#include "ConfigPerGoal.h"
//#include "ConfigUniformSearch.h"

#ifdef SLB_VISUALIZATION
#define SLB_GRAPH StateGraph<SLB_STATE>
#else
#define SLB_GRAPH NoGraph<SLB_STATE>
#endif

#include "core/headers.h"
#include "extensions/headers.h"

SLB_GRAPH buildGraph() {
    SLB_GRAPH g;
#ifndef SLB_VISUALIZATION
    return g;
#endif
    using MyOL = OpenList<SLB_NODE, DefaultPriority, GreaterPriority_SmallG>;
    using MyHeuristic = ZeroHeuristic<SLB_STATE>;
    using MyInstance = Instance<SLB_STATE>;

    Nothing logger;
    auto instance =
        MyInstance(std::vector<SLB_STATE>(1), std::vector<SLB_STATE>(1));
    Astar<MyOL, NoGoalHandler, MyHeuristic, SLB_GRAPH, Nothing> myAstar(
        instance, g, logger);
    myAstar.run();
    return g;
}

void run() {
    if (CMD.spaceInitFileName_isSet())
        SLB_STATE::initSpace(CMD.spaceInitFileName());

    SLB_GRAPH g = buildGraph();

    auto res = readInstancesFile<SLB_STATE>(CMD.instancesFileName());
    int i = -1;
    Stats stats;
    for (auto instance : res) {
        ++i;
        //if (i != 50) continue;
        //std::cerr << std::endl << "# " << i << std::endl;
#ifdef SLB_VISUALIZATION
        if (i != SLB_VISUALIZATION) continue;
#endif
        SLB_LOGGER logger;
        SLB_ALGORITHM alg(instance, g, logger);
        alg.run();
        stats.append(alg.measures(), CMD.perInstance()); // add instance number
                                                         // column only in
                                                         // per-instance mode
// break;
#ifdef SLB_VISUALIZATION
        Visualizer<SLB_NODE, false> vis(g, logger);
        vis.run();
        break;
#endif
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
