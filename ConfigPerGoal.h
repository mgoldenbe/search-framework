//#define VISUALIZATION 5

#define STATE GridMapState<>
#define COST_TYPE double
#define UNIFORM_DOMAIN false
#define NODE_DATA MyNodeDataT<STATE>

#define STATE_SMART_PTR StateSharedPtrT
#define BUCKET_POSITION_TYPE int
#define NODE AStarNode<>

#define ALGORITHM_EVENT AstarEvent<>
// Can't put ALGORITHM_EVENT as default argument in AlgorithmLogger,
// since AlgorithmEvent.h uses AlgorithmLogger.
#define LOGGER AlgorithmLogger<AstarEvent<>>

#define BUILD_GRAPH // Need to build the domain graph
#define BUILD_GRAPH_GOAL_HANDLER = NoGoalHandler<>

// Can't put defaults in OL, since there are several instantiations
#define OL_PRIORITY_TYPE DefaultPriority
#define OL_PRIORITY GreaterPriority_SmallF_LargeG
#define OL_CONTAINER OLMap
#define OL OpenList<>

#define SEARCH_START SingleStartState<>
#define SEARCH_GOAL MultipleGoalStates<>
#define INSTANCE Instance<>
#define NSTARTS 1
#define NGOALS 2

//#define GOAL_HANDLER MultipleGoalHandler
#define GOAL_HANDLER SingleGoalHandler

//#define HEURISTIC MinHeuristicToGoals<INSTANCE, ManhattanHeuristic>
#define HEURISTIC                                                              \
    SimpleHeuristicToGoal<Instance<SingleStartState<>, SingleGoalState<>>,     \
                          OctileHeuristic>

#define RAW_ALGORITHM Astar<>
#define ALGORITHM PerGoal<>

#define VISUAL_EVENT AstarVisualEvent<>
