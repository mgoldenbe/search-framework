// All #includes must be here.
// Otherwise, when we have <... = STATE> in some header, we would have to
// include all state implementations there.
// In the future, the script might generate these automatically.
// For now, just include each new header in the proper place.
#include "utilities.h"
#include "Table.h"
#include "CommandLine.h"

#include "StateNeighbor.h"

#include "Pancake.h"

#include "ExplicitState.h"
#include "GridMap.h"
#include "GridMapState.h"

#include "Instance.h"
#include "Stats.h"

#include "ManagedNode.h"
#include "NodeData.h"
#include "Node.h"

#include "AlgorithmLogger.h"
#include "AstarEvent.h"

#include "OL.h"
#include "OCL.h"
#include "GoalHandler.h"
#include "Heuristic.h"
#include "Astar.h"
#include "PerGoal.h"
#include "Graph.h"
#ifdef VISUALIZATION
#include "VisualLog.h"
#include "AstarVisualEvent.h"
#include "Visualizer.h"
#endif
