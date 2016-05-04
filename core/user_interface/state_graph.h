#ifndef GRAPH_H
#define GRAPH_H

/**
 * @file   state_graph.h
 * @author Meir Goldenberg
 * @date   Wed May  4 20:20:48 2016
 *
 * @brief  Defines the class for storing the state space graph.
 *
 *
 */

using namespace boost;

template <class State> struct NoGraph {
    using StateUniquePtr = std::unique_ptr<const State>;
    using StateSharedPtr = std::shared_ptr<const State>;
    using CostType = typename State::CostType;

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

/// The state space graph used for visualization.
/// \tparam State The state type.
template <class State>
struct StateGraph {
    /// Type of edge cost
    using CostType = typename State::CostType;

    /// Smart pointer to state.
    using StateSharedPtr = std::shared_ptr<const State>;

    /// The actual graph type.
    using Graph =
        boost::adjacency_list<vecS, setS, directedS, StateSharedPtr, CostType>;

    /// Iterator for looping over vertices.
    using VertexIterator = typename graph_traits<Graph>::vertex_iterator;

    /// Vertex identifier.
    using VertexDescriptor = typename graph_traits<Graph>::vertex_descriptor;

    /// Edge identifier.
    using EdgeDescriptor = typename boost::graph_traits<Graph>::edge_descriptor;

    /// Special graph type used for computing the layout. See http://stackoverflow.com/q/33903879/2725810 for the discussion of this issue.
    using LayoutGraph =
        boost::adjacency_list<vecS, vecS, undirectedS, int, double>;

    /// Vertex descriptor for \ref LayoutGraph.
    using LayoutVertexDescriptor =
        typename graph_traits<LayoutGraph>::vertex_descriptor;

    /// Type for vertex coordinates (used for computing the layout)
    using Point = square_topology<>::point_type;

    /// Map for storing the layout, i.e. the coordinates of each vertex.
    using PointMap = std::map<VertexDescriptor, Point>;

    /// Just like \ref PointMap, but using \ref LayoutVertexDescriptor to identify the vertices
    using LayoutPointMap =
        std::map<LayoutVertexDescriptor, Point>;

    /// Computes the iterator range that can be used to range-loop on the
    /// vertices.
    /// \return The iterator range.
    auto vertexRange() const -> decltype(make_iterator_range(
        vertices(std::declval<Graph>()))) {
        return make_iterator_range(vertices(g_));
    }

    /// Computes the iterator range that can be used to range-loop on the
    /// edges.
    /// \return The iterator range.
    auto edgeRange() const -> decltype(make_iterator_range(
        edges(std::declval<Graph>()))) {
        return make_iterator_range(edges(g_));
    }

    /// Computes edge based on two vertex descriptors.
    /// \param from The source vertex
    /// \param to The target vertex
    /// \return The edge descriptor.
    EdgeDescriptor edge(VertexDescriptor from, VertexDescriptor to) const {
        auto res = boost::edge(from, to, g_);
        if (!res.second) assert(0);
        return res.first;
    }

    /// Computes the source vertex of the given edge.
    /// \param ed The edge.
    /// \return The source vertex descriptor.
    VertexDescriptor from(EdgeDescriptor ed) const { return source(ed, g_); }

    /// Computes the target vertex of the given edge.
    /// \param ed The edge descriptor.
    /// \return The target vertex descriptor.
    VertexDescriptor to(EdgeDescriptor ed) const { return target(ed, g_); }

    /// Computes state corresponding to the given vertex.
    /// \param vd The vertex descriptor.
    /// \return Shared pointer to the corresponding state. If \c vd is invalid,
    /// \c nullptr is returned.
    StateSharedPtr state(VertexDescriptor vd) const {
        if (!vd) return nullptr;
        return g_[vd];
    }

    /// Computes the vertex corresponding to the given state.
    /// \param s Shared pointer to state.
    /// \return The vertex descriptor.
    /// \pre The vertex corresponding to \c s must exist.
    VertexDescriptor vertex(const StateSharedPtr &s) const {
        auto it =  stov_.find(s);
        assert(it != stov_.end());
        return it->second;
    }

    /// Computes the edge between the given states.
    /// \param from Shared pointer to the source state.
    /// \param to Shared pointer to the target state.
    /// \return The edge descriptor.
    /// \pre The said edge must exist.
    EdgeDescriptor edge(const StateSharedPtr &from,
                        const StateSharedPtr &to) const {
        return edge(vertex(from), vertex(to));
    }

    /// Computes vertices adjacent to the given vertex.
    /// \param vd The vertex descriptor.
    /// \return Iterator range that can be used to range-loop over the adjacent
    /// vertices of \c vd.
    auto adjacentVertexRange(VertexDescriptor vd) const
        -> decltype(make_iterator_range(
            adjacent_vertices(vd, std::declval<Graph>()))) {
        return make_iterator_range(adjacent_vertices(vd, g_));
    }

    /// Adds a state to the graph.
    /// \param s Smart pointer to the state.
    /// \return The descriptor of the newly added vertex.
    VertexDescriptor add(StateSharedPtr s) {
        auto it = stov_.find(s);
        if (it != stov_.end()) return it->second;
        auto vd = add_vertex(s, g_);
        stov_[s] = vd;
        return vd;
    }

    /// Adds a directed edge between the two given states to the graph. If the
    /// vertex corresponding to the target state \c to does not exist, it is
    /// added as well. The edge is added only if an edge from \c from to \c to
    /// does not exist already.
    /// \param from The source state.
    /// \param to The target state.
    /// \param cost Edge cost.
    void add(const StateSharedPtr &from, const StateSharedPtr to,
             CostType cost) {
        auto source = stov_[from];
        auto target = add(to);
        if (!boost::edge(source, target, g_).second)
            add_edge(source, target, cost, g_);
    }

    /// Dumps the graph contents to std::cerr.
    void dump() const {
        for (auto vd: make_iterator_range(vertices(g_))) {
            std::cerr << *g_[vd] << ":" << std::endl;
            for (auto ed:  make_iterator_range(out_edges(vd, g_))) {
                auto &successor = g_[target(ed, g_)];
                auto &cost = g_[ed];
                std::cerr << "    " << *successor << " (w=" << cost << ")"
                          << std::endl;
            }
        }
    }

    /// Computes a graph layout. Unless additional layout options are turned on,
    /// a randomized circular layout is used.
    /// \param kamadaKawaiFlag If \c true, the Kamada-Kawai layout algorithm is
    /// used.
    /// \param fruchtermanReingoldFlag If \c true, the Fruchterman-Reingold
    /// layout algorithm is used. If \c kamadaKawaiFlag is set as well, then
    /// the Kamada-Kawai algorithm is applied first.
    /// \param cost Edge cost.
    /// \return The graph layout.
    PointMap layout(bool kamadaKawaiFlag = true,
                    bool fruchtermanReingoldFlag = true);
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

#endif
