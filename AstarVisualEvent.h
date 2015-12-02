#ifndef ASTAR_VISUAL_EVENT
#define ASTAR_VISUAL_EVENT

#include "Colors.h"

struct RGB {
    static int red(int color) { return color >> 16; }
    static int green(int color) { return (color >> 8) % 256; }
    static int blue(int color) { return color % 256; }
};

struct VertexStyle {
    Color fillColor;
    Color circleColor;
    int circleWidth;
};

struct VertexStyles {
    static VertexStyle start() {
        return {Color(), Color::NAVY_BLUE, 2};
    }
};

struct EdgeStyle {};

template <class Graph_, class Event>
struct AstarVisualEvent {
    using Graph = Graph_;
    using VertexDescriptor = typename Graph::VertexDescriptor;
    using EdgeDescriptor = typename Graph::EdgeDescriptor;
    using VertexChange = struct {
        VertexDescriptor vd;
        VertexStyle style;
        int lastEventStep; // just put -1 here. The log will need to take care
    };
    using EdgeChange = struct {
        EdgeDescriptor ed;
        EdgeStyle style;
        int lastEventStep; // just put -1 here. The log will need to take care
    };

    AstarVisualEvent(const Graph &g, const Event &e) : g_(g) {
        switch (e.type()) {
        case Event::EventType::ROLE: {
            VertexStyle style;
            switch (e.role()) {
            case Event::StateRole::START:
                style = VertexStyles::start();
                break;
            default:
                ;
            }
            auto s = e.state();
            auto vd = g_.vertex(s);
            vertexChanges_.push_back({vd, style, -1});
            break;
        }
        default:
            ;
        }
    }

    // VisualLog will need to modify the returned value
    std::vector<VertexChange> &vertexChanges() {return vertexChanges_;}
    std::vector<EdgeChange> &edgeChanges() {return edgeChanges_;}

private:
    const Graph &g_;
    std::vector<VertexChange> vertexChanges_;
    std::vector<EdgeChange> edgeChanges_;
};

#endif
