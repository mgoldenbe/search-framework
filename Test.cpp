///@file
///@brief NEED TO MAKE SURE THAT THE CODE IS AS SUCCINCT AS POSSIBLE.

#include "Pancake.h"
#include "GridMap.h"
#include "GridMapState.h"
#include "OL.h"
#include "OCL.h"
#include "GoalHandler.h"
#include "Heuristic.h"
#include "Astar.h"
#include "Graph.h"
#include "AlgorithmLogger.h"
#include "AstarEvent.h"
#include "VisualLog.h"
#include "AstarVisualEvent.h"
#ifndef NO_DRAWER
#include "Visualizer.h"
#endif

//#define STATE_OPTION_PANCAKE

#ifdef STATE_OPTION_PANCAKE
using MyState = Pancake;
#else
using MyState = GridMapState<int>;
#endif
using MyCostType = MyState::CostType;

template <typename State> struct MyNodeDataT : ManagedNode<NodeBase<State>> {
    MyNodeDataT() : responsibleGoal(0) {}
    REFLECTABLE(
        (State)
        responsibleGoal) // the goal that was responsible for the heuristic value
};
using MyNodeData = MyNodeDataT<MyState>;
using MyNode = AStarNode<MyState, MyNodeData>;
using MyAlgorithmEvent = AstarEvent<MyState, MyNodeData>;

//using MyLogger = NoAlgorithmLogger<MyAlgorithmEvent>;
using MyLogger = AlgorithmLogger<MyAlgorithmEvent>;

//using MyGoalHandler = NoGoalHandler<State, Logger>;
using MyGoalHandler = MultipleGoalHandler<MyState, MyLogger>;

using MyUniformSearchOL = OL<MyNode, DefaultPriority, GreaterPriority_SmallG>;
using MyOL = OL<MyNode, DefaultPriority, GreaterPriority_SmallF_LargeG>;

//using MyGraph = NoStateGraph<MyState, MyCostType>;
using MyGraph = StateGraph<MyState, MyCostType>;

using MyVisualEvent = AstarVisualEvent<MyGraph, MyAlgorithmEvent>;
using MyVisualLog = VisualLog<MyLogger, MyVisualEvent>;

#ifdef STATE_OPTION_PANCAKE
using MyHeuristic = MinHeuristicToGoals<MyState, GapHeuristicToGoal>;
#else
using MyHeuristic = MinHeuristicToGoals<MyState, ManhattanHeuristic>;
#endif

void testAstar() {
#ifdef STATE_OPTION_PANCAKE
    Pancake goal1(4), start(goal1);
    start.shuffle();

    Pancake goal2(goal1); goal2.shuffle();
#else
    GridMap<int> m("ost001d.map8");
    //GridMap<int> m("tiny.map8");
    MyState::space(&m);
    MyState start(5000), goal1(5001), goal2(5002);
    //MyState start(0), goal1(1), goal2(2);
#endif
    MyGraph g;
    MyLogger logger1, logger;

    // Build graph
    Astar<MyUniformSearchOL, NoGoalHandler, ZeroHeuristic<MyCostType>, MyGraph,
          MyLogger> myAstar1(start, NoGoalHandler(),
                             ZeroHeuristic<MyCostType>(), g, logger1);
    myAstar1.run();

    // Real search
    std::vector<MyState> myGoals = {goal1, goal2};
    //std::cout << goal1 << " " << goal2 << std::endl;
    auto myGoalHandler = MyGoalHandler(myGoals, logger);

#ifdef STATE_OPTION_PANCAKE
    auto heuristicInstance = GapHeuristicToGoal();
#else
    auto heuristicInstance = ManhattanHeuristic();
#endif
    Astar<MyOL, MyGoalHandler, MyHeuristic, MyGraph, MyLogger> myAstar(
        start, myGoalHandler, MyHeuristic(myGoals, heuristicInstance), g,
        logger);
    myAstar.run();

    /*
    Table t(2);
    MyAlgorithmEvent::dumpTitle(t);
    for (auto &e : logger.events()) {
        e.dump(t);
        t << std::endl;
    }
    std::cout << t << std::endl;
    */
    //g.dump();
    //return;
#ifndef NO_DRAWER
    MyVisualLog visualLog(logger, g);
    Visualizer<MyGraph, MyVisualLog, false> vis(g, visualLog);
    vis.run();
#endif
}

int main() {
    testAstar();
    return 0;
}
