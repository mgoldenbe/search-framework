#ifndef GRAPH
#define GRAPH

#include <iostream>                  // for std::cout
#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

using namespace boost;

template<class StateNeighbor>
struct StateGraph {
    using State = typename StateNeighbor::State;
    using CostType = typename StateNeighbor::CostType;
    using Graph = boost::adjacency_list<vecS, setS, directedS, State, CostType>;
    using VertexIterator = typename graph_traits<Graph>::vertex_iterator;

    void add(const State &s) { add_vertex(s, g_); }

    void add(const std::vector<StateNeighbor> &successors) {
        for (auto el: successors)
            add(el.state());
    }

    void dump() {
        VertexIterator vi, vi_end;
        for (tie(vi, vi_end) = vertices(g_); vi != vi_end; ++vi)
            std::cout << g_[*vi] << std::endl;
    }
private:
    Graph g_;
};

#endif
