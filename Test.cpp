///@file
///@brief NEED TO MAKE SURE THAT THE CODE IS AS SUCCINCT AS POSSIBLE.

//#include CONFIG
#include "Config.h"

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
    using MyHeuristic = ZeroHeuristic<INSTANCE, MyLogger>;

    MyLogger logger;
    auto instance = INSTANCE{};
    Astar<MyOL, INSTANCE, NoGoalHandler, MyHeuristic, GRAPH,
          NoAlgorithmLogger<ALGORITHM_EVENT>> myAstar(instance, g, logger);
    myAstar.run();
    return g;
}

void testAstar() {
#ifdef INIT_SPACE_FROM_FILE
    STATE::initSpace("ost001d.map8");
#endif

    GRAPH g = buildGraph();

    auto res = readInstancesFile<INSTANCE>("instances");
    int i = -1;
    Table statsTable;
    for (auto instance : res) {
        ++i;
#ifdef VISUALIZATION
        if (i != VISUALIZATION) continue;
#endif
        LOGGER logger;
        ALGORITHM alg(instance, g, logger);
        auto res = alg.run();
        if (i == 0) {
            for (auto c : alg.stats()) statsTable << c.name();
            statsTable << "Cost" << std::endl;
        }
        for (auto c : alg.stats()) statsTable << c;
        statsTable << res;
        statsTable << std::endl;

#ifdef VISUALIZATION
        Visualizer<GRAPH, LOGGER, VISUAL_EVENT, false> vis(g, logger);
        vis.run();
        break;
#endif
    }
    std::cout << statsTable;
}

void makeInstances() {
#ifdef INIT_SPACE_FROM_FILE
    STATE::initSpace("ost001d.map8");
#endif
    makeInstancesFile<INSTANCE>(100, "instances");
}

int main() {
    testAstar();
    //makeInstances();
    return 0;
}
