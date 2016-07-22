#ifndef CURRENT_STYLES_H
#define CURRENT_STYLES_H

/// \file current_styles.h
/// \brief The \ref CurrentStyles class.
/// \author Meir Goldenberg

#include "graph_layouts.h"
#include "style.h"

/// A class for storing the current styles of vertices and edges in the visual
/// representation.
/// \tparam State The state type, represents the domain.
template <class State> struct CurrentStyles {
    /// Smart pointer to state.
    using StateSharedPtr = std::shared_ptr<const State>;

    /// The graph type.
    using Graph = StateGraph<State>;

    /// Vertex identifier.
    using VertexDescriptor = typename Graph::VertexDescriptor;

    /// Edge identifier.
    using EdgeDescriptor = typename Graph::EdgeDescriptor;

    /// Initializes  graph
    CurrentStyles(const Graph &g) : g_(g) {}

    /// Returns the current style of the given vertex.
    /// \param vd Vertex identifier.
    /// \return The vertex style.
    /// \pre The vertex must be present in the graph.
    VertexStyle get(VertexDescriptor vd) const {
        try {
            return vertexStyles_.at(vd);
        }
        catch(std::exception &e) {
            std::cerr << "Look-up in vertex styles failed!" << std::endl;
            throw e;
        }
    }

    /// Returns the current style of the given edge.
    /// \param ed Edge identifier.
    /// \return The edge style.
    /// \pre The edge must be present in the graph.
    EdgeStyle get(EdgeDescriptor ed) const {
        return edgeStyles_.at(ed);
    }

    /// Returns the current style of the vertex representing a given state.
    /// \param s Shared pointer to state.
    /// \return The vertex style.
    /// \pre The vertex corresponding to \c s must be present in the graph.
    VertexStyle get(StateSharedPtr s) const {
        return vertexStyles_.at(g_.vertex(s));
    }

    /// Returns the current style of the edge between the given state.
    /// \param from Shared pointer to the source state.
    /// \param to Shared pointer to the target state.
    /// \return The edge style.
    /// \pre The edge must be present in the graph.
    EdgeStyle get(StateSharedPtr from, StateSharedPtr to) const {
        return edgeStyles_.at(g_.edge(from, to));
    }
protected:
    /// The graph.
    const Graph &g_;

    /// Mapping from each vertex to its style.
    std::unordered_map<VertexDescriptor, VertexStyle> vertexStyles_;

    /// Mapping from each edge to its style.
    /// There is no hash function for \ref EdgeDescriptor, hence a sorted map.
    std::map<EdgeDescriptor, EdgeStyle> edgeStyles_;
};

#endif
