#ifndef ASTAR_VISUAL_EVENT
#define ASTAR_VISUAL_EVENT

#include "VisualizationUtilities.h"

struct DefaultAstarStyles {
    static VertexStyle defaultVertexStyle() {return ::defaultVertexStyle();}
    static EdgeStyle defaultEdgeStyle() {return ::defaultEdgeStyle();}
    static void roleStart(VertexStyle &style) {
        style.emphasisColor = Color::VIVID_BLUE;
        style.emphasisWidth = 2;
    }
};

template <class Graph_, class Event, class Styles = DefaultAstarStyles>
struct AstarVisualEvent {
    using Graph = Graph_;
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

    template <typename VisualLog>
    AstarVisualEvent(const Graph &g, const Event &e, const VisualLog log)
        : g_(g) {

        const auto &state = e.state();
        auto vd = g_.vertex(state);

        switch (e.type()) {
        case Event::EventType::ROLE: {
            VertexStyle before = log.vertexStyle(vd);
            VertexStyle now = before;
            switch (e.role()) {
            case Event::StateRole::START:
                Styles::roleStart(now);
                break;
            default:
                ;
            }
            vertexChanges_.push_back({vd, now, before});
            break;
        }
        default:
            ;
        }
    }

    static VertexStyle defaultVertexStyle() {
        return Styles::defaultVertexStyle();
    }
    static EdgeStyle defaultEdgeStyle() {
        return Styles::defaultEdgeStyle();
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

    const std::vector<VertexChange> &vertexChanges() {return vertexChanges_;}
    const std::vector<EdgeChange> &edgeChanges() {return edgeChanges_;}
private:
    const Graph &g_;
    std::vector<VertexChange> vertexChanges_;
    std::vector<EdgeChange> edgeChanges_;
};

#endif
