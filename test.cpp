/// \file
/// \brief The standard main file, which uses the framework for the most common
/// experimentation scenarios -- running a search algorithm for a set of problem
/// instances, running a search algorithm for a single problem instance with
/// visualization and creating a new set of problem instances.
/// \author Meir Goldenberg

#ifdef CONFIG
#include CONFIG
#else
#include "projects/KGoal/Config/min.h"
// //#include "projects/KGoal/ConfigPerGoal.h"
// //#include "projects/KGoal/ConfigUniformSearch.h"
#endif

#include "slb.h"

using namespace slb;

using namespace core;
using namespace ui;
using namespace commandLine;

using namespace ext;
using algorithm::Astar;

using namespace policy;
using namespace generator;
using namespace goalHandler;
using namespace openList;

/// The open list policy of \ref slb::ext::algorithm::Astar for building the state graph of the
/// domain.
/// \tparam As every policy, this one needs to have the search algorithm type
/// template parameter.
template <class MyAlgorithm>
using BuildGraphOL =
    BucketedStdMap_T<MyAlgorithm, SLB_NODE, DefaultOLKeyType, GreaterPriority_SmallG, ext::policy::openList::OLMap>;

/// The generator policy of \ref slb::ext::algorithm::Astar for building the
/// state graph of the domain.
/// \tparam As every policy, this one needs to have the search algorithm type
/// template parameter.
template <class MyAlgorithm>
using BuildGenerator = StatesT<MyAlgorithm, policy::heuristic::Zero>;

/// Builds the state graph of the domain.
/// \return The state graph of the domain.
StateGraph<SLB_STATE> buildGraph() {
    using State = SLB_STATE;
    using MyInstance = Instance<State>;

    auto instance = MyInstance(std::vector<State>(1),
                               std::vector<State>(1), MeasureSet{});
    Astar<false, SLB_NODE, NoGoal, policy::heuristic::Zero, BuildGenerator,
          BuildGraphOL>
        myAstar{instance};
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
        int first = CMD.firstInstance() == -1 ? 0 : CMD.firstInstance();
        int last = CMD.lastInstance() == -1 ? res.size() - 1 : CMD.lastInstance();
        for (int i = first; i <= last; ++i) {
            auto &instance = res[i];
            SLB_ALGORITHM<false> alg(instance);
            alg.run();
            auto instanceMeasures = instance.filteredMeasures();
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
        commandLine::CommandLine<>::instance(
            argc, argv); // to initialize command line

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
