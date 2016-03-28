#ifndef ASTAR_VISUAL_EVENT
#define ASTAR_VISUAL_EVENT

#include <vector>
#include "VisualizationUtilities.h"

struct DefaultAstarStyles {
    static void roleStart(VertexStyle &style) {
        style.emphasisColor = Color::VIVID_GREEN;
        style.emphasisWidthFactor = 0.5;
    }
    static void roleGoal(VertexStyle &style) {
        style.emphasisColor = Color::DEEP_BLUE;
        style.emphasisWidthFactor = 0.5;
    }
    static void roleDoneGoalBegin(VertexStyle &style) {
        style.fillColor = doneVertexColor;
    }
    static void roleDoneGoalBegin(EdgeStyle &style) {
        style.color = Color::VIVID_GREEN;
        style.arrow = true;
    }
    static void beginGenerate(VertexStyle &style) {
        style.fillColor = Color::SUNSHINE_YELLOW;
    }
    static void beginGenerate(EdgeStyle &style) {
        style.color = Color::SUNSHINE_YELLOW;
    }
    static void endGenerate(VertexStyle &style) {
        style.fillColor = Color::PALE_YELLOW;
    }
    static void endGenerate(EdgeStyle &style) {
        style.color = Color::PALE_YELLOW;
    }
    static void selected(VertexStyle &style) {
        style.fillColor = Color::VIVID_PURPLE;
    }
    static void suspendedExpansion(VertexStyle &style) {
        style.fillColor = Color::ORANGE;
    }
    static void resumedExpansion(VertexStyle &style) {
        selected(style);
    }
    static void deniedExpansion(VertexStyle &style) {
        endGenerate(style);
    }
    static void closed(VertexStyle &style) { // for the start node
        if (style.fillColor != doneVertexColor)
            style.fillColor = Color::WARM_BROWN;
    }
    static void closed(VertexStyle &style, EdgeStyle &edgeStyle) {
        if (style.fillColor != doneVertexColor)
            style.fillColor = Color::WARM_BROWN;
        edgeStyle.color = Color::WARM_BROWN;
    }

    static constexpr Color doneVertexColor = Color::VIVID_GREEN;
};

template <class Graph_ = GRAPH, class Event = ALGORITHM_EVENT,
          class Styles = DefaultAstarStyles>
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
    AstarVisualEvent(const Graph &g, const Event &e, VisualLog &log)
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
            case Event::StateRole::GOAL:
                Styles::roleGoal(now);
                break;
            case Event::StateRole::BEGIN_DONE_GOAL: {
                Styles::roleDoneGoalBegin(now);
                bool firstIterationFlag = true;
                typename Event::StateSharedPtr last = nullptr;
                for (auto &s : e.path(state)) {
                    if (!firstIterationFlag) {
                        EdgeDescriptor ed =
                            g_.edge(g_.vertex(last), g_.vertex(s));
                        auto edgeBefore = log.edgeStyle(ed);
                        auto edgeNow = edgeBefore;
                        Styles::roleDoneGoalBegin(edgeNow);
                        edgeChanges_.push_back({ed, edgeNow, edgeBefore});
                    }
                    last = s;
                    firstIterationFlag = false;
                }
                break;
            }
            case Event::StateRole::END_DONE_GOAL: { // reverse all edge changes
                auto lastEvent = log.event(e.step() - 1);
                for (auto &c: lastEvent.edgeChanges_) {
                    auto ed = c.ed;
                    edgeChanges_.push_back(
                        {ed, c.before, log.edgeStyle(ed)});
                }
            }
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
                g_.edge(g_.vertex(e.parent()), vd)};
            //g_.edge(vd, g_.vertex(e.parent()))};
            for (auto ed: eds) {
                EdgeStyle edgeBefore = log.edgeStyle(ed);
                EdgeStyle edgeNow = edgeBefore;
                switch(e.type()) {
                case Event::EventType::BEGIN_GENERATE:
                    Styles::beginGenerate(now);
                    Styles::beginGenerate(edgeNow);
                    break;
                case Event::EventType::END_GENERATE:
                    Styles::endGenerate(now);
                    Styles::endGenerate(edgeNow);
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
        case Event::EventType::SUSPENDED_EXPANSION:
            Styles::suspendedExpansion(now);
            break;
        case Event::EventType::RESUMED_EXPANSION:
            Styles::resumedExpansion(now);
            break;
        case Event::EventType::DENIED_EXPANSION:
            Styles::deniedExpansion(now);
            break;
        default:
            ;
        }
        vertexChanges_.push_back({vd, now, before});
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
        for (auto &ec : edgeChanges_) {
            drawer.drawEdge(g_.from(ec.ed), g_.to(ec.ed),
                            beforeFlag ? ec.now : ec.before, true);
            drawer.drawEdge(g_.from(ec.ed), g_.to(ec.ed),
                            beforeFlag ? ec.before : ec.now);
            drawer.drawVertex(g_.from(ec.ed));
            drawer.drawVertex(g_.to(ec.ed));
        }
        for (auto &vc : vertexChanges_)
            drawer.drawVertex(vc.vd, beforeFlag ? vc.before : vc.now);
    }
private:
    const Graph &g_;
    std::vector<VertexChange> vertexChanges_;
    std::vector<EdgeChange> edgeChanges_;
};

#endif
