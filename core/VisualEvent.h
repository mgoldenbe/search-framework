#ifndef ASTAR_VISUAL_EVENT
#define ASTAR_VISUAL_EVENT

template <class Node = NODE>
struct VisualEvent {
    using State = typename Node::State;
    using Graph = StateGraph<State>;
    using VertexDescriptor = typename Graph::VertexDescriptor;
    using EdgeDescriptor = typename Graph::EdgeDescriptor;
    using VertexStyle = ::VertexStyle;
    using EdgeStyle = ::EdgeStyle;
    using VertexChange = struct {
        VertexDescriptor vd;
        VertexStyle now;
        VertexStyle before;
    };
    using EdgeChange = struct {
        EdgeDescriptor ed;
        EdgeStyle now;
        EdgeStyle before;
    };
    using Event = std::shared_ptr<Events::Base<Node>>;

    template <typename VisualLog>
    VisualEvent(const Graph &g, const Event &e, VisualLog &log)
        : g_(g) {
        auto changes = e->visualChanges(log.currentStyles());
        for (auto vc: changes.vChanges) {
            auto before = log.currentStyles().get(vc.s);
            vertexChanges_.push_back(
                VertexChange{g.vertex(vc.s), vc.now, before});
        }
        for (auto ac: changes.aChanges) {
            auto before = log.currentStyles().get(ac.from, ac.to);
            edgeChanges_.push_back(
                EdgeChange{g.edge(ac.from, ac.to), ac.now, before});
        }
    }

    // Returns style now and style before
    std::pair<VertexStyle, VertexStyle> vertexChange(VertexDescriptor vd) {
        for (auto el: vertexChanges_)
            if (el.vd == vd) return std::make_pair(el.now, el.before);
        assert(0);
    }
    std::pair<EdgeStyle, EdgeStyle> edgeChange(EdgeDescriptor ed) {
        for (auto el: edgeChanges_)
            if (el.ed == ed) return std::make_pair(el.now, el.before);
        assert(0);
    }

    const std::vector<VertexChange> &vertexChanges() const {
        return vertexChanges_;
    }
    const std::vector<EdgeChange> &edgeChanges() const { return edgeChanges_; }

    template <class Drawer> void draw(Drawer &drawer, bool beforeFlag = false) {
        for (auto &vc : vertexChanges_)
            drawer.drawVertex(vc.vd, beforeFlag ? vc.before : vc.now);
        for (auto &ec : edgeChanges_)
            drawer.drawEdge(g_.from(ec.ed), g_.to(ec.ed),
                            beforeFlag ? ec.before : ec.now);
    }
private:
    const Graph &g_;
    std::vector<VertexChange> vertexChanges_;
    std::vector<EdgeChange> edgeChanges_;
};

#endif
