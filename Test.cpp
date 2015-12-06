#include "Pancake.h"
#include "OL.h"
#include "OCL.h"
#include "GoalHandler.h"
#include "Astar.h"
#include "Graph.h"
#include "AlgorithmLogger.h"
#include "AstarEvent.h"
#include "VisualLog.h"
#include "AstarVisualEvent.h"
#ifndef NO_DRAWER
#include "Drawer.h"
#endif

using State = Pancake;
using CostType = State::CostType;
using Node = AStarNode<State>;

template <class State>
using GoalHandlerT = SingleGoalHandler<State>;
//using GoalHandlerT = NoGoalHandler<State>;
using GoalHandler = GoalHandlerT<State>;

using MyBFSOL = OL<Node, DefaultPriority, GreaterPriority_SmallG>;
using MyOL = OL<Node, DefaultPriority, GreaterPriority_SmallF_LargeG>;

template <class State, typename CostType>
using GraphT = StateGraph<State, CostType>;
//using GraphT = NoGraph<State, CostType>;
using Graph = GraphT<State, CostType>;

using Event =
    AstarEvent<Node::State, Node::NodeData>;
using MyLogger = AlgorithmLogger<Event>;

using VisualEvent = AstarVisualEvent<Graph, Event>;
using MyVisualLog = VisualLog<MyLogger, VisualEvent>;

void testAstar() {
    Pancake goal(4), start(goal);
    start.shuffle();
    Graph g;
    MyLogger logger1, logger;

    // Build graph
    Astar<MyBFSOL, GapHeuristic, GoalHandlerT, GraphT, MyLogger>
        myAstar1(start, GoalHandler(goal), GapHeuristic(), g, logger1);
    myAstar1.run();

    // Real search
    Astar<MyOL, GapHeuristic, GoalHandlerT, GraphT, MyLogger>
        myAstar(start, GoalHandler(goal), GapHeuristic(), g, logger);
    myAstar.run();
    logger.dump();
    //g.dump();
#ifndef NO_DRAWER
    MyVisualLog visualLog(logger, g);
    Drawer<Graph, MyVisualLog> d(g, visualLog);
    d.run();
#endif
}

int main() {
    testAstar();
    return 0;
}
