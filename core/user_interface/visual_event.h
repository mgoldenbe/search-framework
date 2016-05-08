#ifndef VISUAL_EVENT_H
#define VISUAL_EVENT_H

#include "event_base.h"

template <class Node = SLB_NODE>
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
        bool hideLastFlag =
            (e->eventType() == Events::EventType::HIDE_LAST_EVENT);
        const Event &myEvent = (hideLastFlag ? e->previousEvent() : e);

        if (hideLastFlag) // to pass correct current styles to
                          // myEvent->visualChanges
            log.stepBackward();
        auto changes = myEvent->visualChanges(log.currentStyles());
        if (hideLastFlag) log.stepForward(); // restored
        for (auto vc: changes.vChanges)
            vertexChanges_.push_back(
                hideLastFlag ? VertexChange{g.vertex(vc.s), vc.before, vc.now}
                             : VertexChange{g.vertex(vc.s), vc.now, vc.before});
        for (auto ac : changes.aChanges)
            edgeChanges_.push_back(
                hideLastFlag
                    ? EdgeChange{g.edge(ac.from, ac.to), ac.before, ac.now}
                    : EdgeChange{g.edge(ac.from, ac.to), ac.now, ac.before});

        // We don't have a way to draw two arcs in opposite directions, so each
        // each event needs to affect both directions. One example when this is
        // needed is the Events::NotGenerated event generated in Astar
        int sizeNow = edgeChanges_.size();
        for (int i = 0; i != sizeNow; i++) {
            auto ec = edgeChanges_[i];
            ec.ed = g.inverse(ec.ed);
            edgeChanges_.push_back(ec);
        }

        /*
        // dumping all changes
        std::cerr << "Step " << log.step() << std::endl;
        for (const auto &ec : edgeChanges_)
            std::cerr << *(g.state(g.from(ec.ed))) << "->"
                      << *(g.state(g.to(ec.ed)))
                      << "    before: " << static_cast<int>(ec.before.color)
                      << "  now: " << static_cast<int>(ec.now.color)
                      << std::endl;
        */
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
