#ifndef GRAPH
#define GRAPH

#include <iostream>                  // for std::cout
#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each
#include <unordered_map>
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
    using VertexDescriptor = typename graph_traits<Graph>::vertex_descriptor;

    std::pair<VertexDescriptor, bool> add(const State &s) {
        auto it = stov_.find(s);
        if (it != stov_.end()) return std::make_pair(it->second, false);
        auto vd = add_vertex(s, g_);
        stov_[s] = vd;
        return std::make_pair(vd, true);
    }

    void add(const State &parent, const StateNeighbor &n) {
        auto res = add(n.stateCopy());
        if (res.second) add_edge(stov_[parent], res.first, n.cost(), g_);
    }

    void dump() {
        for (auto vd: make_iterator_range(vertices(g_))) {
            std::cout << g_[vd] << ":" << std::endl;
            for (auto ed:  make_iterator_range(out_edges(vd, g_))) {
                auto &successor = g_[target(ed, g_)];
                auto &cost = g_[ed];
                std::cout << "    " << successor << " (w=" << cost << ")"
                          << std::endl;
            }
        }
    }

private:
    Graph g_;
    std::unordered_map<State, VertexDescriptor, StateHash<State>> stov_;
};

#endif
