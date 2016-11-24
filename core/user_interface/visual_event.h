#ifndef VISUAL_EVENT_H
#define VISUAL_EVENT_H

/// \file
/// \brief The \ref slb::core::ui::VisualEvent class.
/// \author Meir Goldenberg

#include "event_base.h"

namespace slb {
namespace core {
namespace ui {

/// The event for visualization.
/// \tparam State The state type, represents the domain.
template <class State>
struct VisualEvent {
    /// \name Types related to the graph of the domain.
    /// \see \ref StateGraph.
    /// @{
    using Graph = StateGraph<State>;
    using VertexDescriptor = typename Graph::VertexDescriptor;
    using EdgeDescriptor = typename Graph::EdgeDescriptor;
    /// @}

    using VertexStyle = ui::VertexStyle; ///< See \ref VertexStyle.
    using EdgeStyle = ui::EdgeStyle; ///< See \ref EdgeStyle.

    /// Description of change of visual representation of a single vertex.
    struct VertexChange {
        VertexDescriptor vd; ///< The vertex identifier.
        VertexStyle now;     ///< The new representation.
        VertexStyle before;  ///< The previous representation.
    };
    /// Description of change of visual representation of a single edge.
    struct EdgeChange {
        EdgeDescriptor ed; ///< The edge identifier.
        EdgeStyle now;     ///< The new representation.
        EdgeStyle before;  ///< The previous representation.
    };

    /// Initializes a visual event based on an event generated by the search
    /// algorithm.
    /// \tparam VisualLog The visual log type.
    /// \param g The (partial) domain graph.
    /// \param e The event generated by the search algorithm.
    /// \param log The visual log.
    template <typename VisualLog>
    VisualEvent(const Graph &g, const typename VisualLog::AlgorithmEvent &e,
                VisualLog &log)
        : g_(g) {
        using Event = typename VisualLog::AlgorithmEvent;
        bool hideLastFlag =
            (e->eventType() == EventType::HIDE_LAST_EVENT);
        const Event &myEvent = (hideLastFlag ? e->lastStateEvent() : e);

        if (hideLastFlag) // to pass correct current styles to
                          // myEvent->visualChanges
            log.stepBackward();
        auto changes = myEvent->visualChanges(log.currentStyles());
        if (hideLastFlag) log.stepForward(); // restored
        for (auto vc: changes.vChanges)
            vertexChanges_.push_back(
                hideLastFlag ? VertexChange{g.vertex(vc.s), vc.before, vc.now}
                             : VertexChange{g.vertex(vc.s), vc.now, vc.before});
        for (auto ac : changes.eChanges)
            edgeChanges_.push_back(
                hideLastFlag
                    ? EdgeChange{g.edge(ac.from, ac.to), ac.before, ac.now}
                    : EdgeChange{g.edge(ac.from, ac.to), ac.now, ac.before});

        // We don't have a way to draw two edges in opposite directions, so each
        // each event needs to affect both directions. One example when this is
        // needed is the ext::event::NotGenerated event generated in Astar
        int sizeNow = edgeChanges_.size();
        for (int i = 0; i != sizeNow; i++) {
            auto ec = edgeChanges_[i];
            // can't have arrow in both directions
            ec.now.arrow = ec.before.arrow = false;
            ec.ed = g.inverse(ec.ed);
            if (ec.ed != g.null_edge) edgeChanges_.push_back(ec);
        }

        // // dumping all changes (for debugging purposes only)
        // std::cerr << "Step " << log.step() << std::endl;
        // for (const auto &ec : edgeChanges_)
        //     std::cerr << *(g.state(g.from(ec.ed))) << "->"
        //               << *(g.state(g.to(ec.ed)))
        //               << "    before: " << static_cast<int>(ec.before.color)
        //               << "  now: " << static_cast<int>(ec.now.color)
        //               << std::endl;
    }

    /// Returns style now and style before the visual event of the given vertex.
    /// \param vd The vertex identifier.
    /// \return The pair: style now and style before the visual event.
    std::pair<VertexStyle, VertexStyle> vertexChange(VertexDescriptor vd) {
        for (auto el: vertexChanges_)
            if (el.vd == vd) return std::make_pair(el.now, el.before);
        assert(0);
    }

    /// Returns style now and style before the visual event of the given edge.
    /// \param ed The edge identifier.
    /// \return The pair: style now and style before the visual event.
    std::pair<EdgeStyle, EdgeStyle> edgeChange(EdgeDescriptor ed) {
        for (auto el: edgeChanges_)
            if (el.ed == ed) return std::make_pair(el.now, el.before);
        assert(0);
    }

    /// Returns all changes of vertex representations made by the visual event.
    /// \return Vector of changes of vertex representations made by the visual
    /// event.
    const std::vector<VertexChange> &vertexChanges() const {
        return vertexChanges_;
    }

    /// Returns all changes of edge representations made by the visual event.
    /// \return Vector of changes of edge representations made by the visual
    /// event.
    const std::vector<EdgeChange> &edgeChanges() const { return edgeChanges_; }

    /// Visualizes the changes corresponding to the visual event.
    /// \tparam Drawer The drawer type.
    /// \param drawer The drawer.
    /// \param beforeFlag If \c true, the inverse of the visual event is
    /// visualized.
    template <class Drawer> void draw(Drawer &drawer, bool beforeFlag = false) {
        for (auto &vc : vertexChanges_)
            drawer.drawVertex(vc.vd, beforeFlag ? vc.before : vc.now);
        for (auto &ec : edgeChanges_)
            drawer.drawEdge(g_.from(ec.ed), g_.to(ec.ed),
                            beforeFlag ? ec.before : ec.now);
    }
private:
    const Graph &g_; ///< The graph.

    /// Changes of visual representations of vertices.
    std::vector<VertexChange> vertexChanges_;

    /// Changes of visual representations of edges.
    std::vector<EdgeChange> edgeChanges_;
};

} // namespace
} // namespace
} // namespace

#endif
