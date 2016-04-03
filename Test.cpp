///@file
///@brief NEED TO MAKE SURE THAT THE CODE IS AS SUCCINCT AS POSSIBLE.

//#include CONFIG
#include "Config.h"
//#include "ConfigPerGoal.h"

#ifdef VISUALIZATION
#define GRAPH StateGraph<STATE, COST_TYPE>
#else
#define GRAPH NoGraph<STATE, COST_TYPE>
#endif

#include "Headers.h"

// http://stackoverflow.com/a/7712341/2725810
struct CommandLine {
    CommandLine(int argc, char **argv)
        : cmd_("The Generic Search Library", ' ', "0.1"),
          instancesFileName_("i", "instances", "File with instances", true, "",
                             "string", cmd_),
          spaceInitFileName_("s", "space",
                             "File to initialize an explicit space", false, "",
                             "string", cmd_),
          nInstances_("n", "nInstances", "Number of instances to create", false,
                      -1, "int", cmd_),
          perInstance_("p", "perInstance", "Output per-instance stats", cmd_,
                       false) {
        try {
            cmd_.parse(argc, argv);
        }
        catch (TCLAP::ArgException &e) // catch any exceptions
        {
            std::cerr << "error: " << e.error() << " for arg " << e.argId()
                      << std::endl;
        }
    }

    std::string instancesFileName() { return instancesFileName_.getValue(); }

    bool spaceInitFileName_isSet() { return spaceInitFileName_.isSet(); }
    std::string spaceInitFileName() { return spaceInitFileName_.getValue(); }

    int nInstances() { return nInstances_.getValue(); }

    bool perInstance() { return perInstance_.getValue(); }

private:
    TCLAP::CmdLine cmd_;
    TCLAP::ValueArg<std::string> instancesFileName_;
    TCLAP::ValueArg<std::string> spaceInitFileName_;
    TCLAP::ValueArg<int> nInstances_;
    TCLAP::SwitchArg perInstance_;
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

void run(CommandLine &cmd) {
    if (cmd.spaceInitFileName_isSet())
        STATE::initSpace(cmd.spaceInitFileName());

    GRAPH g = buildGraph();

    auto res = readInstancesFile<INSTANCE>(cmd.instancesFileName());
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
        stats.append(alg.measures(), cmd.perInstance()); // add instance number
                                                         // column only in
                                                         // per-instance mode
// break;
#ifdef VISUALIZATION
        Visualizer<GRAPH, LOGGER, VISUAL_EVENT, false> vis(g, logger);
        vis.run();
        break;
#endif
    }
    if (!cmd.perInstance())
        stats = stats.average();
    Table statsTable;
    stats.dump(statsTable);
    std::cout << statsTable;
}

void makeInstances(CommandLine &cmd) {
    if (cmd.spaceInitFileName_isSet())
        STATE::initSpace(cmd.spaceInitFileName());
    makeInstancesFile<INSTANCE>(cmd.nInstances(), cmd.instancesFileName());
}

int main(int argc, char **argv) {
    // need to catch exceptions
    CommandLine cmd(argc, argv);
    if (cmd.nInstances() != -1)
        makeInstances(cmd);
    else
        run(cmd);
    return 0;
}
