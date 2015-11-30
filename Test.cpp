#include "Pancake.h"
#include "OL.h"
#include "OCL.h"
#include "Astar.h"
#include "Graph.h"
#ifndef NO_DRAWER
#include "Drawer.h"
#endif

using State = Pancake;
using CostType = State::CostType;
using Node = AStarNode<State>;

template <class State>
//using GoalHandlerT = SingleGoalHandler<State>;
using GoalHandlerT = NoGoalHandler<State>;
using GoalHandler = GoalHandlerT<State>;

using MyOL = OL<Node, DefaultPriority, GreaterPriority_SmallG>;

template <class State, typename CostType>
using GraphT = StateGraph<State, CostType>;
//using GraphT = NoGraph<State, CostType>;
using Graph = GraphT<State, CostType>;

void testAstar() {
    Pancake goal(4), start(goal);
    start.shuffle();
    Graph g;
    Astar<MyOL, GapHeuristic, GoalHandlerT, GraphT>
        myAstar(start, GoalHandler(goal), g);
    myAstar.run();
    //g.dump();
#ifndef NO_DRAWER
    Drawer<Graph> d(g);
    d.run();
#endif
}

int main() {
    testAstar();
    return 0;
}
