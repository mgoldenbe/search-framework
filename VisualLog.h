#ifndef VISUAL_LOG
#define VISUAL_LOG

#include <map>
#include <unordered_map>

template <class VisualEvent>
struct NoVisualLog {
    using Graph = typename VisualEvent::Graph;
    using VertexDescriptor = typename Graph::VertexDescriptor;
    using EdgeDescriptor = typename Graph::EdgeDescriptor;

    void log(VisualEvent e) {
        (void)e;
    }
};

template <class VisualEvent>
struct VisualLog {
    using Graph = typename VisualEvent::Graph;
    using VertexDescriptor = typename Graph::VertexDescriptor;
    using EdgeDescriptor = typename Graph::EdgeDescriptor;
    using VertexStyle = typename VisualEvent::VertexStyle;
    using EdgeStyle = typename VisualEvent::EdgeStyle;

    template <class AlgorithmLog>
    VisualLog(const AlgorithmLog &log_, const Graph &g) : g_(g) {
        for (auto vd : g_.vertexRange())
            vertexStyles_[vd] = VisualEvent::defaultVertexStyle();
        for (auto ed : g_.edgeRange())
            edgeStyles_[ed] = VisualEvent::defaultEdgeStyle();
        for (auto &e: log_.events())
            log(VisualEvent(g_, e, (*this)));
    }

    const VertexStyle &vertexStyle(VertexDescriptor vd) const {
        auto it = vertexStyles_.find(vd);
        if (it == vertexStyles_.end()) assert(0);
        return it->second;
    }
    const EdgeStyle &edgeStyle(EdgeDescriptor ed) const {
        auto it = edgeStyles_.find(ed);
        if (it == edgeStyles_.end()) assert(0);
        return it->second;
    }

private:
    const Graph &g_;

    // -1 means no previous event
    std::unordered_map<VertexDescriptor, VertexStyle> vertexStyles_;

    // EdgeDescriptor cannot be a key for unordered_map
    std::map<EdgeDescriptor, EdgeStyle> edgeStyles_;

    std::vector<VisualEvent> events_;

    void log(VisualEvent e) {
        for (auto &vertexChange: e.vertexChanges())
            vertexStyles_[vertexChange.vd] = vertexChange.now;
        for (auto &edgeChange : e.edgeChanges())
            edgeStyles_[edgeChange.ed] = edgeChange.now;
        events_.push_back(e);
    }
};

#endif
