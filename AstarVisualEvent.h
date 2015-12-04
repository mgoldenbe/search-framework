#ifndef ASTAR_VISUAL_EVENT
#define ASTAR_VISUAL_EVENT

#include "VisualizationUtilities.h"

struct DefaultAstarStyles {
    static VertexStyle defaultVertexStyle() {return ::defaultVertexStyle();}
    static EdgeStyle defaultEdgeStyle() {return ::defaultEdgeStyle();}
    static void roleStart(VertexStyle &style) {
        style.emphasisColor = Color::VIVID_GREEN;
        style.emphasisWidth = defaultVertexStyle().size/2;
    }
    static void beginGenerate(VertexStyle &vertexStyle, EdgeStyle &edgeStyle) {
        vertexStyle.fillColor = Color::SUNSHINE_YELLOW;
        edgeStyle.color = Color::SUNSHINE_YELLOW;
    }
    static void endGenerate(VertexStyle &vertexStyle, EdgeStyle &edgeStyle) {
        vertexStyle.fillColor = Color::PALE_YELLOW;
        edgeStyle.color = Color::PALE_YELLOW;
    }
    static void selected(VertexStyle &style) {
        style.fillColor = Color::VIVID_PURPLE;
    }
    static void closed(VertexStyle &style) { // for the start node
        style.fillColor = Color::WARM_BROWN;
    }
    static void closed(VertexStyle &style, EdgeStyle &edgeStyle) {
        style.fillColor = Color::WARM_BROWN;
        edgeStyle.color = Color::WARM_BROWN;
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
        VertexStyle before = log.vertexStyle(vd);
        VertexStyle now = before;

        switch (e.type()) {
        case Event::EventType::ROLE:
            switch (e.role()) {
            case Event::StateRole::START:
                Styles::roleStart(now);
                break;
            default:
                ;
            }
            break;
        case Event::EventType::BEGIN_GENERATE:
        case Event::EventType::END_GENERATE:
        case Event::EventType::CLOSED: {
            if (e.type() == Event::EventType::CLOSED)
                if (!e.parent()) {
                    Styles::closed(now);
                    break;
                }
            std::vector<EdgeDescriptor> eds = {
                g_.edge(g_.vertex(e.parent()), vd),
                g_.edge(vd, g_.vertex(e.parent()))};
            for (auto ed: eds) {
                EdgeStyle edgeBefore = log.edgeStyle(ed);
                EdgeStyle edgeNow = edgeBefore;
                switch(e.type()) {
                case Event::EventType::BEGIN_GENERATE:
                    Styles::beginGenerate(now, edgeNow);
                    break;
                case Event::EventType::END_GENERATE:
                    Styles::endGenerate(now, edgeNow);
                    break;
                case Event::EventType::CLOSED:
                    Styles::closed(now, edgeNow);
                    break;
                default: assert(0);
                }
                edgeChanges_.push_back({ed, edgeNow, edgeBefore});
            }
            break;
        }
        case Event::EventType::SELECTED:
            Styles::selected(now);
            break;
        default:
            ;
        }
        vertexChanges_.push_back({vd, now, before});
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

    const std::vector<VertexChange> &vertexChanges() const {
        return vertexChanges_;
    }
    const std::vector<EdgeChange> &edgeChanges() const { return edgeChanges_; }

private:
    const Graph &g_;
    std::vector<VertexChange> vertexChanges_;
    std::vector<EdgeChange> edgeChanges_;
};

#endif
