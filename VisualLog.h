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

template <class AlgorithmLog, class VisualEvent>
struct VisualLog {
    using Graph = typename VisualEvent::Graph;
    using VertexDescriptor = typename Graph::VertexDescriptor;
    using EdgeDescriptor = typename Graph::EdgeDescriptor;
    using VertexStyle = typename VisualEvent::VertexStyle;
    using EdgeStyle = typename VisualEvent::EdgeStyle;

    VisualLog(const AlgorithmLog &log, const Graph &g) : log_(log), g_(g) {
        for (auto vd : g_.vertexRange())
            vertexStyles_[vd] = VisualEvent::defaultVertexStyle();
        for (auto ed : g_.edgeRange())
            edgeStyles_[ed] = VisualEvent::defaultEdgeStyle();

        step_ = 0;
        for (auto &e: log.events()) {
            auto ve = VisualEvent(g_, e, (*this));
            this->log(ve);
        }
        // rewind to the beginning
        while (step_ > 0)
            prev();
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

    void next() {
        if (step_ >= events_.size()) return;
        auto e = events_[step_++];
        applyEvent(e);
    }

    void prev() {
        if (step_ <= 0) return;
        auto e = events_[--step_];
        unApplyEvent(e);
    }

private:
    const AlgorithmLog &log_;
    const Graph &g_;

    // -1 means no previous event
    std::unordered_map<VertexDescriptor, VertexStyle> vertexStyles_;

    // EdgeDescriptor cannot be a key for unordered_map
    std::map<EdgeDescriptor, EdgeStyle> edgeStyles_;

    std::vector<VisualEvent> events_;
    int step_; // the event to be applied when next() is called.

    void applyEvent(const VisualEvent &e) {
        for (auto &vertexChange: e.vertexChanges())
            vertexStyles_[vertexChange.vd] = vertexChange.now;
        for (auto &edgeChange : e.edgeChanges())
            edgeStyles_[edgeChange.ed] = edgeChange.now;
    }

    void unApplyEvent(const VisualEvent &e) {
        for (auto &vertexChange: e.vertexChanges())
            vertexStyles_[vertexChange.vd] = vertexChange.before;
        for (auto &edgeChange : e.edgeChanges())
            edgeStyles_[edgeChange.ed] = edgeChange.before;
    }

    void log(const VisualEvent &e) {
        applyEvent(e);
        events_.push_back(e);
        step_++;
    }
};

#endif
