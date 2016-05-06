#ifndef GRAPH_LAYOUTS_H
#define GRAPH_LAYOUTS_H

/// \file graph_layouts.h
/// \brief Implementation of layout computation methods of the \ref StateGraph
/// class.
/// \author Meir Goldenberg

#include "state_graph.h"
    template <class MyMap>
    void dumpLayout(MyMap layout) {
    for (auto &el : layout)
        std::cout << (el.second)[0] << " " << (el.second)[1] << std::endl;
}

template <class State>
void StateGraph<State>::initLayoutGraph() {
    if (num_vertices(lg_)) return; // already initialized

    int i = 0;
    for (auto vd : vertexRange()) {
        auto lvd = add_vertex(lg_);
        lg_[lvd] = i++;
        graphToLayout_[vd] = lvd;
        layoutToGraph_[lvd] = vd;
    }

    // // The case for succeeding Kamada-Kawai with circle layout for 3-pancake
    // for (auto lfrom: make_iterator_range(vertices(lg)))
    //     for (auto lto: make_iterator_range(vertices(lg))) {
    //         if (lg[lfrom] == 0 && lg[lto] == 1) add_edge(lfrom, lto, 1, lg);
    //         if (lg[lfrom] == 0 && lg[lto] == 2) add_edge(lfrom, lto, 1, lg);
    //         if (lg[lfrom] == 1 && lg[lto] == 4) add_edge(lfrom, lto, 1, lg);
    //         if (lg[lfrom] == 2 && lg[lto] == 3) add_edge(lfrom, lto, 1, lg);
    //         if (lg[lfrom] == 3 && lg[lto] == 5) add_edge(lfrom, lto, 1, lg);
    //         if (lg[lfrom] == 4 && lg[lto] == 5) add_edge(lfrom, lto, 1, lg);
    //     }
    // // The case for failing Kamada-Kawai with circle layout for 3-pancake
    // for (auto lfrom: make_iterator_range(vertices(lg)))
    //     for (auto lto: make_iterator_range(vertices(lg))) {
    //         if (lg[lfrom] == 0 && lg[lto] == 1) add_edge(lfrom, lto, 1, lg);
    //         if (lg[lfrom] == 0 && lg[lto] == 2) add_edge(lfrom, lto, 1, lg);
    //         if (lg[lfrom] == 1 && lg[lto] == 3) add_edge(lfrom, lto, 1, lg);
    //         if (lg[lfrom] == 2 && lg[lto] == 4) add_edge(lfrom, lto, 1, lg);
    //         if (lg[lfrom] == 3 && lg[lto] == 5) add_edge(lfrom, lto, 1, lg);
    //         if (lg[lfrom] == 4 && lg[lto] == 5) add_edge(lfrom, lto, 1, lg);
    //     }

    for (auto from : vertexRange()) {
        for (auto to : adjacentVertexRange(from)) {
            auto lfrom = graphToLayout_[from], lto = graphToLayout_[to];
            if (!boost::edge(lfrom, lto, lg_).second) {
                // std::cout << "Connecting: " << lg_[lfrom] << "-->" <<
                // lg_[lto] << std::endl;
                auto edgePair = boost::edge(from, to, g_);
                add_edge(lfrom, lto, (double)(g_[edgePair.first]), lg_);
            }
        }
    }
}

template <class State>
void StateGraph<State>::initBaseLayout(bool circularFlag) {
    if (baseLayout_.size()) return; // already initialized
    boost::associative_property_map<LayoutPointMap> temp(baseLayout_);
    if (circularFlag)
        circle_graph_layout(lg_, temp, 50.0);
    else {
        minstd_rand gen;
        rectangle_topology<> rect_top(gen, 0, 0, 100, 100);
        random_graph_layout(lg_, temp, rect_top);
    }
    //dumpLayout(baseLayout_);
}

template <class State> void StateGraph<State>::randomizeBaseLayout() {
    shuffleMap(baseLayout_);
}

template <class State>
// http://stackoverflow.com/q/33912929/2725810
typename StateGraph<State>::PointMap
StateGraph<State>::layout(bool kamadaKawaiFlag, bool fruchtermanReingoldFlag) {
    PointMap pointMapRes;

    initLayoutGraph();
    initBaseLayout();
    randomizeBaseLayout();
    LayoutPointMap layoutPointMapRes{baseLayout_};

    boost::associative_property_map<LayoutPointMap> temp(layoutPointMapRes);
    using Topology = square_topology<>;
    // minstd_rand gen;
    // Topology topology(gen, 100.0);
    Topology topology(100.0);

    if (kamadaKawaiFlag) {
        kamada_kawai_spring_layout(lg_, temp, get(edge_bundle, lg_), topology,
                                   side_length(100.0),
                                   // kamada_kawai_done<LayoutGraph>());
                                   layout_tolerance<double>(0.0001));

        // std::cout << "Done KK" << std::endl;
    }

    if (fruchtermanReingoldFlag) {
        std::vector<Topology::point_difference_type> displacements(
            num_vertices(lg_));
        fruchterman_reingold_force_directed_layout(
            lg_, temp, topology, square_distance_attractive_force(),
            square_distance_repulsive_force(), all_force_pairs(),
            linear_cooling<double>(100),
            make_iterator_property_map(displacements.begin(),
                                       get(vertex_index, lg_),
                                       Topology::point_difference_type()));
    }

    // It can actually spoil the perfect symmetry for 4-pancake
    // fruchterman_reingold_force_directed_layout(
    //      lg, temp, topology,
    //      attractive_force(square_distance_attractive_force())
    //          .cooling(linear_cooling<double>(100)));

    for (auto el : layoutPointMapRes)
        pointMapRes[layoutToGraph_[el.first]] = el.second;

    return pointMapRes;
}

// // Custom stopping condition for Kamada-Kawai algorithm.
// // Sometimes the algorithm with this condition diverges.
// // Not sure where I got this code.
// template <typename Graph> struct kamada_kawai_done {
//     using VertexDescriptor =
//         typename boost::graph_traits<Graph>::vertex_descriptor;

//     kamada_kawai_done() : last_delta() {}

//     bool operator()(double delta_p, VertexDescriptor vd, const Graph & /*g*/,
//                     bool global) {
//         if (global) { // delta_p is the maximal energy in the system
//             double diff = last_delta - delta_p;
//             if (diff < 0) diff = -diff;
//             std::cout << "global=true;  delta_p: " << delta_p << std::endl;
//             last_delta = delta_p;
//             return diff < 0.01;
//         } else { // delta_p is the energy of the vertex being moved

//             std::cout << "global=false;  vd: " << vd
//                       << "   delta_p: " << delta_p << std::endl;
//             /*
//             // Catch when it diverges by fluctuating
//             auto &myHistory = history[vd];
//             for (auto last_venergy : myHistory)
//                 if (fabs(delta_p - last_venergy) < 0.01) return true;
//             myHistory.push_back(delta_p);
//             */
//             return delta_p < 0.01;
//         }
//     }

//     double last_delta;
//     std::map<VertexDescriptor, std::vector<double>> history;
// };

#endif
