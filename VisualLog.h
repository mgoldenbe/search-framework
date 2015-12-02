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

    template <class AlgorithmLog>
    VisualLog(const AlgorithmLog &log_, const Graph &g) : g_(g) {
        for (auto &e: log_.events())
            log(VisualEvent(g_, e));
    }

private:
    const Graph &g_;

    // -1 means no previous event
    std::unordered_map<VertexDescriptor, int> vertexToLastEventStep_;

    // unordered_map with EdgeDescriptor does not seem to work
    std::map<EdgeDescriptor, int> edgeToLastEventStep_;

    std::vector<VisualEvent> events_;

    const VisualEvent &getLastVertexEvent(VertexDescriptor vd) {
        return events_[vertexToLastEventStep_[vd]];
    }
    const VisualEvent &getLastEdgeEvent(EdgeDescriptor ed) {
        return events_[edgeToLastEventStep_[ed]];
    }
    void log(VisualEvent e) {
        for (auto &vertexChange: e.vertexChanges()) {
            auto vd = vertexChange.vd;
            vertexToLastEventStep_[vd] = events_.size();
            auto it = vertexToLastEventStep_.find(vd);
            vertexChange.lastEventStep = -1;
            if (it != vertexToLastEventStep_.end())
                vertexChange.lastEventStep = it->second;
        }
        for (auto &edgeChange : e.edgeChanges()) {
            auto ed = edgeChange.ed;
            edgeToLastEventStep_[ed] = events_.size();
            auto it = edgeToLastEventStep_.find(ed);
            edgeChange.lastEventStep = -1;
            if (it != edgeToLastEventStep_.end())
                edgeChange.lastEventStep = it->second;
        }
        events_.push_back(e);
    }
};

#endif
