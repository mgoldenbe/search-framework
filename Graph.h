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

template <typename Graph> struct kamada_kawai_done {
    using VertexDescriptor =
        typename boost::graph_traits<Graph>::vertex_descriptor;

    kamada_kawai_done() : last_delta() {}

    bool operator()(double delta_p, VertexDescriptor vd, const Graph & /*g*/,
                    bool global) {
        if (global) { // delta_p is the maximal energy in the system
            double diff = last_delta - delta_p;
            if (diff < 0) diff = -diff;
            std::cout << "global=true;  delta_p: " << delta_p << std::endl;
            last_delta = delta_p;
            return diff < 0.01;
        }
        else { // delta_p is the energy of the vertex being moved

            std::cout << "global=false;  vd: " << vd
                      << "   delta_p: " << delta_p << std::endl;
            /*
            // Catch when it diverges by fluctuating
            auto &myHistory = history[vd];
            for (auto last_venergy : myHistory)
                if (fabs(delta_p - last_venergy) < 0.01) return true;
            myHistory.push_back(delta_p);
            */
            return delta_p < 0.01;
        }
    }

    double last_delta;
    std::map<VertexDescriptor, std::vector<double>> history;
};

template <class State, typename CostType>
struct StateGraph {
    using StateSharedPtr = std::shared_ptr<const State>;
    using Graph =
        boost::adjacency_list<vecS, setS, directedS, StateSharedPtr, CostType>;
    using VertexIterator = typename graph_traits<Graph>::vertex_iterator;
    using VertexDescriptor = typename graph_traits<Graph>::vertex_descriptor;
    using EdgeDescriptor = typename boost::graph_traits<Graph>::edge_descriptor;

    //const Graph &graph() {return g_;}

    auto vertexRange() const -> decltype(make_iterator_range(
        vertices(std::declval<Graph>()))) {
        return make_iterator_range(vertices(g_));
    }

    auto edgeRange() const -> decltype(make_iterator_range(
        edges(std::declval<Graph>()))) {
        return make_iterator_range(edges(g_));
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

    void add(const StateSharedPtr &parent, const StateSharedPtr n, CostType cost) {
        auto from = stov_[parent];
        auto to = add(n);
        if (!edge(from, to, g_).second) add_edge(from, to, cost, g_);
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

    // http://stackoverflow.com/q/33912929/2725810
    template <class PointMap> PointMap layout() const {
        PointMap res;

        // Make a copy into a graph that is easier to deal with:
        //     -- vecS for vertex set, so there is index map
        //     -- double for edge weights
        using LayoutGraph =
            boost::adjacency_list<vecS, vecS, undirectedS, int, double>;
        using LayoutVertexDescriptor =
            typename graph_traits<LayoutGraph>::vertex_descriptor;
        std::map<VertexDescriptor, LayoutVertexDescriptor> myMap;
        std::map<LayoutVertexDescriptor, VertexDescriptor> myReverseMap;
        LayoutGraph lg;
        int i = 0;
        for (auto vd : vertexRange()) {
            auto lvd = add_vertex(lg);
            lg[lvd] = i++;
            myMap[vd] = lvd;
            myReverseMap[lvd] = vd;
        }
        /* // The case for succeeding Kamada-Kawai with circle layout for 3-pancake
        for (auto lfrom: make_iterator_range(vertices(lg)))
            for (auto lto: make_iterator_range(vertices(lg))) {
                if (lg[lfrom] == 0 && lg[lto] == 1) add_edge(lfrom, lto, 1, lg);
                if (lg[lfrom] == 0 && lg[lto] == 2) add_edge(lfrom, lto, 1, lg);
                if (lg[lfrom] == 1 && lg[lto] == 4) add_edge(lfrom, lto, 1, lg);
                if (lg[lfrom] == 2 && lg[lto] == 3) add_edge(lfrom, lto, 1, lg);
                if (lg[lfrom] == 3 && lg[lto] == 5) add_edge(lfrom, lto, 1, lg);
                if (lg[lfrom] == 4 && lg[lto] == 5) add_edge(lfrom, lto, 1, lg);
            }
        */
        /* // The case for failing Kamada-Kawai with circle layout for 3-pancake
        for (auto lfrom: make_iterator_range(vertices(lg)))
            for (auto lto: make_iterator_range(vertices(lg))) {
                if (lg[lfrom] == 0 && lg[lto] == 1) add_edge(lfrom, lto, 1, lg);
                if (lg[lfrom] == 0 && lg[lto] == 2) add_edge(lfrom, lto, 1, lg);
                if (lg[lfrom] == 1 && lg[lto] == 3) add_edge(lfrom, lto, 1, lg);
                if (lg[lfrom] == 2 && lg[lto] == 4) add_edge(lfrom, lto, 1, lg);
                if (lg[lfrom] == 3 && lg[lto] == 5) add_edge(lfrom, lto, 1, lg);
                if (lg[lfrom] == 4 && lg[lto] == 5) add_edge(lfrom, lto, 1, lg);
            }
        */


        for (auto from: vertexRange()) {
            for (auto to: adjacentVertexRange(from)) {
                auto lfrom = myMap[from], lto = myMap[to];
                if (!edge(lfrom, lto, lg).second) {
                    // std::cout << "Connecting: " << lg[lfrom] << "-->" <<
                    // lg[lto] << std::endl;
                    auto edgePair = edge(from, to, g_);
                    add_edge(lfrom, lto, (double)(g_[edgePair.first]),
                             lg);
                }
            }
        }

        using LayoutPointMap =
            std::map<LayoutVertexDescriptor, square_topology<>::point_type>;
        LayoutPointMap intermediateResults;
        boost::associative_property_map<LayoutPointMap> temp(
            intermediateResults);

        minstd_rand gen;
        rectangle_topology<> rect_top(gen, 0, 0, 100, 100);
        random_graph_layout(lg, temp, rect_top);

        // Kamada-Kawai depends on position in the circle
        // E.g. for this graph: 0->1, 0->2, 1->3, 2->4, 3->5, 4->5
        //circle_graph_layout(lg, temp, 50.0);

        kamada_kawai_spring_layout(lg, temp, get(edge_bundle, lg),
                                   square_topology<>(100.0), side_length(100.0),
                                   // kamada_kawai_done<LayoutGraph>());
                                   layout_tolerance<double>(0.0001));
        /*
        for (auto &el : intermediateResults)
            std::cout << (el.second)[0] << " " << (el.second)[1] << std::endl;
        */
        //std::cout << "Done KK" << std::endl;

        using Topology = square_topology<>;
        Topology topology(gen, 100.0);
        /*
        std::vector<Topology::point_difference_type> displacements(
            num_vertices(lg));
        fruchterman_reingold_force_directed_layout
            (lg, temp,
             topology,
             square_distance_attractive_force(),
             square_distance_repulsive_force(),
             all_force_pairs(),
             linear_cooling<double>(100),
             make_iterator_property_map(displacements.begin(),
                                        get(vertex_index, lg),
                                        Topology::point_difference_type()));
        */
        /* It actually spoils the perfect symmetry for 4-pancake
        fruchterman_reingold_force_directed_layout(
             lg, temp, topology,
             attractive_force(square_distance_attractive_force())
                 .cooling(linear_cooling<double>(100)));
        */
        for (auto el: intermediateResults)
            res[myReverseMap[el.first]] = el.second;

        return res;
    }

    VertexDescriptor vertex(const StateSharedPtr &s) const {
        auto it =  stov_.find(s);
        assert(it != stov_.end());
        return it->second;
    }
private:
    Graph g_;
    std::unordered_map<StateSharedPtr, VertexDescriptor,
                       StateSharedPtrHash<State>> stov_;
};

#endif
