#ifndef CURRENT_STYLES_H
#define CURRENT_STYLES_H

template <class State> struct CurrentStyles {
    using StateSharedPtr = std::shared_ptr<const State>;
    using Graph = StateGraph<State>;
    using VertexDescriptor = typename Graph::VertexDescriptor;
    using EdgeDescriptor = typename Graph::EdgeDescriptor;

    VertexStyle get(StateSharedPtr s) const {
        return vertexStyles_.at(g_.vertex(s));
    }
    EdgeStyle get(StateSharedPtr from, StateSharedPtr to) const {
        return edgeStyles_.at(g_.edge(from, to));
    }
    CurrentStyles(const Graph &g) : g_(g) {}
protected:
    const Graph &g_;
    // -1 means no previous event
    std::unordered_map<VertexDescriptor, VertexStyle> vertexStyles_;

    // EdgeDescriptor cannot be a key for unordered_map
    std::map<EdgeDescriptor, EdgeStyle> edgeStyles_;
};

#endif
