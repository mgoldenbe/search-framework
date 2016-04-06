///@file
///@brief NEED TO MAKE SURE THAT THE CODE IS AS SUCCINCT AS POSSIBLE.

//#include CONFIG
//#include "Config.h"
#include "ConfigPerGoal.h"

#ifdef VISUALIZATION
#define GRAPH StateGraph<STATE, COST_TYPE>
#else
#define GRAPH NoGraph<STATE, COST_TYPE>
#endif

#include "Headers.h"

GRAPH buildGraph() {
    GRAPH g;
#ifndef VISUALIZATION
    return g;
#endif
    using MyLogger = NoAlgorithmLogger<ALGORITHM_EVENT>;
    using MyOL = OpenList<NODE, DefaultPriority, GreaterPriority_SmallG>;
    using MyHeuristic = ZeroHeuristic<STATE, MyLogger>;
    using MyInstance = Instance<STATE>;

    MyLogger logger;
    auto instance = MyInstance(std::vector<STATE>(1), std::vector<STATE>(1));
    Astar<MyOL, NoGoalHandler, MyHeuristic, GRAPH,
          NoAlgorithmLogger<ALGORITHM_EVENT>> myAstar(instance, g, logger);
    myAstar.run();
    return g;
}

void run() {
    if (CMD.spaceInitFileName_isSet())
        STATE::initSpace(CMD.spaceInitFileName());

    GRAPH g = buildGraph();

    auto res = readInstancesFile<STATE>(CMD.instancesFileName());
    int i = -1;
    Stats stats;
    for (auto instance : res) {
        ++i;
        //if (i != 18) continue;
        //std::cerr << std::endl << "Solving instace " << i << std::endl;
#ifdef VISUALIZATION
        if (i != VISUALIZATION) continue;
#endif
        LOGGER logger;
        ALGORITHM alg(instance, g, logger);
        alg.run();
        stats.append(alg.measures(), CMD.perInstance()); // add instance number
                                                         // column only in
                                                         // per-instance mode
// break;
#ifdef VISUALIZATION
        Visualizer<GRAPH, LOGGER, VISUAL_EVENT, false> vis(g, logger);
        vis.run();
        break;
#endif
    }
    if (!CMD.perInstance())
        stats = stats.average();
    Table statsTable;
    stats.dump(statsTable);
    std::cout << statsTable;
}

void makeInstances() {
    if (CMD.spaceInitFileName_isSet())
        STATE::initSpace(CMD.spaceInitFileName());
    makeInstancesFile<STATE>(CMD.instancesFileName());
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
