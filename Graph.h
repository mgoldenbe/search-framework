#ifndef GRAPH
#define GRAPH

#include <iostream>                  // for std::cout
#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each
#include <map>
#include <unordered_map>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/random_layout.hpp>
#include <boost/graph/circle_layout.hpp>
#include <boost/graph/kamada_kawai_spring_layout.hpp>
#include <boost/graph/fruchterman_reingold.hpp>
#include <boost/property_map/transform_value_property_map.hpp>
//#include <boost/test/unit_test.hpp> // BOOST_CHECK

#include "utilities.h"

using namespace boost;

template <class State, typename CostType> struct NoGraph {
    using StateUniquePtr = std::unique_ptr<const State>;
    using StateSharedPtr = std::shared_ptr<const State>;

    void add(const StateUniquePtr &s) const {(void)s;}
    void add(const StateSharedPtr &s) const {(void)s;}

    void add(const StateUniquePtr &parent, const StateUniquePtr &n, CostType cost) const {
        (void)parent;
        (void)n;
        (void)cost;
    }
    void add(const StateSharedPtr &parent, const StateSharedPtr &n, CostType cost) const {
        (void)parent;
        (void)n;
        (void)cost;
    }

    void dump() { std::cout << "NoGraph!" << std::endl; }
};

template <class State, typename CostType>
struct StateGraph {
    using StateSharedPtr = std::shared_ptr<const State>;
    using Graph =
        boost::adjacency_list<vecS, setS, directedS, StateSharedPtr, CostType>;
    using VertexIterator = typename graph_traits<Graph>::vertex_iterator;
    using VertexDescriptor = typename graph_traits<Graph>::vertex_descriptor;
    using EdgeDescriptor = typename boost::graph_traits<Graph>::edge_descriptor;

    // http://stackoverflow.com/q/33903879/2725810
    using LayoutGraph =
        boost::adjacency_list<vecS, vecS, undirectedS, int, double>;
    using LayoutVertexDescriptor =
        typename graph_traits<LayoutGraph>::vertex_descriptor;
    using Point = square_topology<>::point_type;
    using PointMap = std::map<VertexDescriptor, Point>;
    using LayoutPointMap =
        std::map<LayoutVertexDescriptor, Point>;

    auto vertexRange() const -> decltype(make_iterator_range(
        vertices(std::declval<Graph>()))) {
        return make_iterator_range(vertices(g_));
    }

    auto edgeRange() const -> decltype(make_iterator_range(
        edges(std::declval<Graph>()))) {
        return make_iterator_range(edges(g_));
    }

    EdgeDescriptor edge(VertexDescriptor from, VertexDescriptor to) const {
        auto res = boost::edge(from, to, g_);
        if (!res.second) assert(0);
        return res.first;
    }

    VertexDescriptor from(EdgeDescriptor ed) const { return source(ed, g_); }
    VertexDescriptor to(EdgeDescriptor ed) const { return target(ed, g_); }

    StateSharedPtr state(VertexDescriptor vd) const {
        if (!vd) return nullptr;
        return g_[vd];
    }

    auto adjacentVertexRange(VertexDescriptor vd) const
        -> decltype(make_iterator_range(
            adjacent_vertices(vd, std::declval<Graph>()))) {
        return make_iterator_range(adjacent_vertices(vd, g_));
    }

    VertexDescriptor add(StateSharedPtr s) {
        auto it = stov_.find(s);
        if (it != stov_.end()) return it->second;
        auto vd = add_vertex(s, g_);
        stov_[s] = vd;
        return vd;
    }

    void add(const StateSharedPtr &parent, const StateSharedPtr n,
             CostType cost) {
        auto from = stov_[parent];
        auto to = add(n);
        if (!boost::edge(from, to, g_).second) add_edge(from, to, cost, g_);
    }

    void dump() const {
        for (auto vd: make_iterator_range(vertices(g_))) {
            std::cout << *g_[vd] << ":" << std::endl;
            for (auto ed:  make_iterator_range(out_edges(vd, g_))) {
                auto &successor = g_[target(ed, g_)];
                auto &cost = g_[ed];
                std::cout << "    " << *successor << " (w=" << cost << ")"
                          << std::endl;
            }
        }
    }

    PointMap layout(bool kamadaKawaiFlag = true,
                    bool fruchtermanReingoldFlag = true);

    VertexDescriptor vertex(const StateSharedPtr &s) const {
        auto it =  stov_.find(s);
        assert(it != stov_.end());
        return it->second;
    }
private:
    Graph g_;
    std::unordered_map<StateSharedPtr, VertexDescriptor,
                       StateSharedPtrHash<State>> stov_;

    std::map<VertexDescriptor, LayoutVertexDescriptor> graphToLayout_;
    std::map<LayoutVertexDescriptor, VertexDescriptor> layoutToGraph_;
    LayoutGraph lg_;
    LayoutPointMap baseLayout_;

    void initLayoutGraph();
    void initBaseLayout(bool circularFlag = true);
    void randomizeBaseLayout();
};

#include "GraphLayout.h"

#endif
