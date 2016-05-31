
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

/**
 * @brief Builds the domain graph
 *
 *
 * @return The domain graph
 */
StateGraph<SLB_STATE> buildGraph() {
    using MyOL = OpenList<SLB_NODE, DefaultOLKeyType, GreaterPriority_SmallG>;
    using MyHeuristic = ZeroHeuristic<SLB_STATE>;
    using MyInstance = Instance<SLB_STATE>;

    auto instance =
        MyInstance(std::vector<SLB_STATE>(1), std::vector<SLB_STATE>(1));
    Astar<false, SLB_NODE, NoGoalHandler, MyHeuristic, MyOL> myAstar(instance);
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
        SLB_ALGORITHM<true> alg(res[CMD.visualizeInstance()]);
        alg.run();
        stats.append(alg.measures(), CMD.perInstance());
        auto g = buildGraph();
        Visualizer<SLB_NODE> vis(g, alg.log());
        vis.run();
    } else {
        for (auto instance : res) {
            Nothing logger; (void)logger;
            SLB_ALGORITHM<false> alg(instance);
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
