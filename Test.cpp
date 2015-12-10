///@file
///@brief NEED TO MAKE SURE THAT THE CODE IS AS SUCCINCT AS POSSIBLE.

#include "Pancake.h"
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
#include "Drawer.h"
#endif

using MyState = Pancake;
using MyCostType = MyState::CostType;

template <typename State> struct MyNodeDataT : public NodeBase<State> {
    MyNodeDataT() : heuristicGoal(0) {}
    State heuristicGoal; // the goal the was responsible for the heuristic value
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

using MyHeuristic = MinHeuristicToGoals<Pancake, GapHeuristicToGoal>;

void testAstar() {
    Pancake goal1(4), start(goal1);
    start.shuffle();

    Pancake goal2(goal1); goal2.shuffle();
    // std::cout << start << " " << start1 << " " << start.gapHeuristic() << " "
    //           << start1.gapHeuristic() << " " << mh(goal) << std::endl;
    // return;

    MyGraph g;
    MyLogger logger1, logger;

    // Build graph
    Astar<MyUniformSearchOL, NoGoalHandler, ZeroHeuristic<MyCostType>, MyGraph,
          MyLogger> myAstar1(start, NoGoalHandler(),
                             ZeroHeuristic<MyCostType>(), g, logger1);
    myAstar1.run();

    // Real search
    std::vector<MyState> myGoals = {goal1, goal2};
    std::cout << goal1 << " " << goal2 << std::endl;
    auto myGoalHandler = MyGoalHandler(myGoals, logger);
    Astar<MyOL, MyGoalHandler, MyHeuristic, MyGraph, MyLogger> myAstar(
        start, myGoalHandler, MyHeuristic(myGoals, GapHeuristicToGoal()), g,
        logger);
    myAstar.run();
    //logger.dump();
    //return;
    //g.dump();
#ifndef NO_DRAWER
    MyVisualLog visualLog(logger, g);
    Drawer<MyGraph, MyVisualLog> d(g, visualLog);
    d.run();
#endif
}

int main() {
    testAstar();
    return 0;
}
