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

#include <tclap/CmdLine.h>
struct CommandLine {
    CommandLine(int argc, char **argv)
        : cmd_("The Generic Search Library", ' ', "0.1"),
          instancesFileName_("i", "instances", "File with instances", true, "",
                             "string", cmd_),
          nInstances_("n", "nInstances", "Number of instances to create", false,
                      -1, "int", cmd_) {
        try {
            cmd_.parse(argc, argv);
        }
        catch (TCLAP::ArgException &e) // catch any exceptions
        {
            std::cerr << "error: " << e.error() << " for arg " << e.argId()
                      << std::endl;
        }
    }

    std::string instancesFileName() {
        return instancesFileName_.getValue();
    }

    int nInstances() {
        return nInstances_.getValue();
    }

private:
    TCLAP::CmdLine cmd_;
    TCLAP::ValueArg<std::string> instancesFileName_;
    TCLAP::ValueArg<int> nInstances_;
};

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

void testAstar(CommandLine &cmd) {
#ifdef INIT_SPACE_FROM_FILE
    STATE::initSpace("ost001d.map8");
#endif

    GRAPH g = buildGraph();

    auto res = readInstancesFile<INSTANCE>(cmd.instancesFileName());
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

void makeInstances(CommandLine &cmd) {
#ifdef INIT_SPACE_FROM_FILE
    STATE::initSpace("ost001d.map8");
#endif
    makeInstancesFile<INSTANCE>(cmd.nInstances(), cmd.instancesFileName());
}

int main(int argc, char **argv) {
    // need to catch exceptions
    CommandLine cmd(argc, argv);
    if (cmd.nInstances() != -1)
        makeInstances(cmd);
    else
        testAstar(cmd);
    return 0;
}
