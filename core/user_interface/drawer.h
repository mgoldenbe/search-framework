#ifndef DRAWER_H
#define DRAWER_H

#include "visual_log.h"

/// \file
/// \brief The \ref Drawer class and the types used for tag dispatch to compute
/// the layout of the (partial) domain graph.
/// \author Meir Goldenberg

/// \defgroup LayoutTraits Layout Traits
/// Checking whether the State has a function for computing layout.
/// Used for tag dispatch in \ref computePointMap_ "Drawer::computePointMap_"
/// @{

/// Declared only if \c State does not have a function for computing layout.
template <class State, typename = void_t<>>
struct has_layout : std::false_type {};

/// Declared only if \c State has a function for computing layout.
template <class State>
struct has_layout<State,
                  void_t<decltype(std::declval<State>().visualLocation(
                      std::declval<double &>(), std::declval<double &>()))>>
    : std::true_type {};

/// Evaluates to \c void only if \c State has a function for computing layout.
template <class State>
using HasLayout = typename std::enable_if<has_layout<State>::value>::type;

/// Evaluates to \c void only if \c State does not have a function for computing
/// layout.
template <class State>
using HasNoLayout = typename std::enable_if<!has_layout<State>::value>::type;

/// @}

// http://stackoverflow.com/a/33421942/2725810
// http://tronche.com/gui/x/xlib/events/keyboard-pointer/keyboard-pointer.html#XButtonEvent
// http://www.lemoda.net/c/xlib-resize/   // Draw only on Expose event!
/// Draws the (partial) domain graph with given styles for visual representation
/// of vertices and edges.
/// \tparam Node The search node type, which is used here to refer to the visual
/// log.
template <class Node> struct Drawer {
    /// The state type, represents the domain.
    using State = typename Node::State;

    /// The domain graph type.
    using Graph = StateGraph<State>;

    /// The log of visual events.
    using MyVisualLog = VisualLog<Node>;

    /// Vertex identifier.
    using VertexDescriptor = typename Graph::VertexDescriptor;

    /// Type for vertex coordinates (used for computing the layout)
    using Point = typename Graph::Point;

    /// Map for storing the layout, i.e. the coordinates of each vertex.
    using PointMap = typename Graph::PointMap;

    /// Initializes the drawer with the given (partial) domain graph and log of
    /// visual events.
    Drawer(Graph &g, const MyVisualLog &log) : g_(g), log_(log) {
        makeLayout();
    }

    /// Computes vertex in the (partial) domain graph based on point
    /// coordinates. This function is used to implement clickable vertices.
    /// \param x The x-coordinate.
    /// \param y The y-coordinate.
    /// \return Identifier of the vertex corresponding to the given coordinates.
    /// If there is no corresponding vertex, \c nullptr is returned.
    const VertexDescriptor coordsToVD(double x, double y) const {
        for (auto &el : pointMap_) {
            auto vd = el.first;
            auto point = el.second;
            switch (log_.get(vd).shape) {
            case VertexShape::CIRCLE: {
                double myDistance = gu::distance({point[0], point[1]}, {x, y});
                if (myDistance <=
                    log_.get(vd).sizeFactor * VertexStyle::sizeBase)
                    return vd;
                break;
            }
            default:
                assert(0);
            }
        }
        return nullptr;
    }

    /// Computes the layout of the (partial) domain graph and scales it to the
    /// current scale.
    /// \param drawFlag If \c true, then \ref draw is called.
    void makeLayout(bool drawFlag = false) {
        graphics_.restore();
        computePointMap_();
        graphics_.sizeX = graphics_.windowXSize;
        graphics_.sizeY = graphics_.windowYSize;
        scaleLayout_(graphics_.sizeX, graphics_.sizeY);
        if (drawFlag) draw();
    }

    /// Returns the graphics object.
    /// \return Reference to the graphics object.
    Graphics &graphics() { return graphics_; }

    /// Draws all vertices and edges with styles corresponding to the current
    /// state in the log of visual events.
    void draw() {
        GroupLock lock{true, graphics_, true};
        (void)lock;
        auto cr = graphics_.cr;
        cairo_set_source_rgb(cr, 56, 128, 4);

        cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);

        // Draws vertices
        for (auto vd : g_.vertexRange()) drawVertex(vd);

        // Draws edges with default color first, so they are in the background
        for (int depth = 0; depth <= 1; ++depth) {
            for (auto from : g_.vertexRange()) {
                for (auto to : g_.adjacentVertexRange(from)) {
                    auto ed = g_.edge(from, to);
                    auto style = log_.get(ed);
                    if ((style.color != EdgeStyle::defaultColor) == depth)
                        drawEdge(from, to);
                }
            }
        }
    }

    /// Draws the given vertex with the given representation style.
    /// \param vd The vertex identifier.
    /// \param style The visual representation style.
    void drawVertex(VertexDescriptor vd, const VertexStyle &style) {
        assert(pointMap_.find(vd) != pointMap_.end());
        fillVertex(vd, style);
        emphasizeVertex(vd, style);
        if (showLabels_) labelVertex(vd, style);
    }

    /// Draws the given vertex with the representation style corresponding to
    /// the current state of the log of visual events.
    /// \param vd The vertex identifier.
    void drawVertex(VertexDescriptor vd) { drawVertex(vd, log_.get(vd)); }

    /// Draws the given edge with the given representation style.
    /// \param from The edge-source vertex identifier.
    /// \param to The edge-target vertex identifier.
    /// \param style The visual representation style.
    void drawEdge(VertexDescriptor from, VertexDescriptor to,
                  const EdgeStyle &style) {
        assert(pointMap_.find(from) != pointMap_.end());
        assert(pointMap_.find(to) != pointMap_.end());

        auto cr = graphics_.cr;
        Color ec = style.color;
        if (ec == Color::NOVAL) return;
        cairo_set_source_rgb(cr, rgb::red(ec), rgb::green(ec), rgb::blue(ec));
        cairo_set_line_width(cr, style.widthFactor * EdgeStyle::widthBase);
        gu::Circle cFrom{gu::Point{pointMap_.at(from)[0], pointMap_.at(from)[1]},
                         VertexStyle::sizeBase};
        gu::Circle cTo{gu::Point{pointMap_.at(to)[0], pointMap_.at(to)[1]},
                       VertexStyle::sizeBase};
        connectCircles(cr, cFrom, cTo);
        if (style.arrow)
            inscribePolygon(cr, cFrom, 3, angle(cFrom.center, cTo.center),
                            true);
        // If the edge is one-way, the draw an arrow in the middle
        if (g_.inverse(g_.edge(from, to)) == g_.null_edge) {
            gu::Circle c{gu::pointOnLine(cFrom.center, cTo.center, 0.5),
                         VertexStyle::sizeBase};
            inscribePolygon(cr, c, 3, angle(cFrom.center, cTo.center), true);
        }
        if (showLabels_) labelEdge(from, to, style);
    }

    /// Draws the given edge with the representation style corresponding to
    /// the current state of the log of visual events.
    /// \param from The edge-source vertex identifier.
    /// \param to The edge-target vertex identifier.
    void drawEdge(VertexDescriptor from, VertexDescriptor to) {
        assert(pointMap_.find(from) != pointMap_.end());
        assert(pointMap_.find(to) != pointMap_.end());

        auto ed = g_.edge(from, to);
        drawEdge(from, to, log_.get(ed));
    }

    /// Sets the mode for either showing or hiding the labels.
    /// \param flag If \c true, the labels will be shown. They
    /// will be hidden otherwise.
    void showLabels(bool flag) { showLabels_ = flag; draw(); }

private:
    /// The (partial) domain graph.
    Graph &g_;

    /// The log of visual events.
    const MyVisualLog &log_;

    /// The layout.
    PointMap pointMap_;

    /// The graphics object holding all the information needed for drawing.
    Graphics graphics_;

    /// Indicates wither the vertex and edge labels should be shown.
    bool showLabels_ = false;

    /// @{
    /// \anchor computePointMap_
    /// \name Computing the layout.
    /// Uses SFINAE to use the \c visualLocation function of State it has one or
    /// automatic layout otherwise.
    /// See \ref HasNoLayout<State> and HasLayout<State>.
    template <class State = State> HasNoLayout<State> computePointMap_() {
        pointMap_ = g_.layout(true, true);
    }

    template <class State = State> HasLayout<State> computePointMap_() {
        for (auto vd : g_.vertexRange()) {
            auto state = g_.state(vd);
            double x, y;
            state->visualLocation(x, y);
            Point myPoint;
            myPoint[0] = x;
            myPoint[1] = y;
            pointMap_[vd] = myPoint;
        }
    }
    /// @}

    /// Fills inside the vertex according to the given style.
    /// \param vd The vertex identifier.
    /// \param style The visual presentation style for the vertex.
    void fillVertex(VertexDescriptor vd, const VertexStyle &style) {
        auto cr = graphics_.cr;
        Color fc = style.fillColor;
        if (fc == Color::NOVAL) return;
        cairo_set_source_rgb(cr, rgb::red(fc), rgb::green(fc), rgb::blue(fc));
        cairo_set_line_width(cr, 0.0);
        cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
        switch (style.shape) {
        case VertexShape::CIRCLE:
            cairo_arc(cr, pointMap_.at(vd)[0], pointMap_.at(vd)[1],
                      style.sizeFactor * VertexStyle::sizeBase, 0, 2 * M_PI);
            break;
        default:
            assert(0);
        }
        cairo_fill(cr);
        cairo_stroke(cr);
    }

    /// Draws the emphasis mark of the vertex (if there is one)
    /// according to the given style.
    /// \param vd The vertex identifier.
    /// \param style The visual presentation style for the vertex.
    void emphasizeVertex(VertexDescriptor vd, const VertexStyle &style) {
        auto cr = graphics_.cr;
        Color ec = style.emphasisColor;
        if (ec == Color::NOVAL) return;
        cairo_set_source_rgb(cr, rgb::red(ec), rgb::green(ec), rgb::blue(ec));
        switch (style.shape) {
        case VertexShape::CIRCLE:
            cairo_arc(cr, pointMap_.at(vd)[0], pointMap_.at(vd)[1],
                      style.sizeFactor * (1.0 - style.emphasisSizeFactor) *
                          VertexStyle::sizeBase,
                      0, 2 * M_PI);
            break;
        default:
            assert(0);
        }
        cairo_fill(cr);
        cairo_stroke(cr);
    }

    /// Labels the vertex.
    /// \param vd The vertex identifier.
    /// \param style The visual presentation style for the vertex.
    void labelVertex(VertexDescriptor vd, const VertexStyle &style) {
        // Compute label
        auto state = g_.state(vd);
        auto label = state->visualLabel();
        if (!label.size()) return;

        // Compute and set color
        auto cr = graphics_.cr;
        Color fc = style.fillColor;
        cairo_set_source_rgb(cr, rgb::cred(fc), rgb::cgreen(fc), rgb::cblue(fc));

        double vx = pointMap_.at(vd)[0];
        double vy = pointMap_.at(vd)[1];
        double size = style.sizeFactor * VertexStyle::sizeBase;

        drawText(cr, label, size, {vx, vy});
    }

    /// Labels the edge.
    /// \param from The edge-source vertex identifier.
    /// \param to The edge-target vertex identifier.
    /// \param style The visual representation style.
    void labelEdge(VertexDescriptor from, VertexDescriptor to,
                   const EdgeStyle &style) {
        // Compute label
        auto fromState = g_.state(from);
        auto toState = g_.state(to);
        auto label = fromState->visualLabel(*toState);
        if (!label.size()) return;

        // Compute and set color
        auto cr = graphics_.cr;
        Color c = style.color;
        cairo_set_source_rgb(cr, rgb::cred(c), rgb::cgreen(c), rgb::cblue(c));

        // Position of text center and size of text
        double vx = (pointMap_.at(from)[0] + pointMap_.at(to)[0]) / 2;
        double vy = (pointMap_.at(from)[1] + pointMap_.at(to)[1]) / 2;
        double size = VertexStyle::sizeBase;

        drawText(cr, label, size, {vx, vy});
    }

    /// Computes a p-percentile of edge pixel-lengths. p must be between 0 and 1.
    /// If p == 0, then the pixel-length of the shortest edge is returned.
    /// If p == 1, the pixel-length of the longest edge is returned.
    /// \return The p-percentile of edge pixel-lengths.
    double distancePercentile(double p) {
        assert(p >= 0 && p <= 1.0);
        std::vector<double> dd;
        for (auto ed : g_.edgeRange()) {
            auto from = g_.from(ed), to = g_.to(ed);
            auto fromPoint = pointMap_.at(from), toPoint = pointMap_.at(to);
            dd.push_back(gu::distance({fromPoint[0], fromPoint[1]},
                                      {toPoint[0], toPoint[1]}));
        }
        sort(dd.begin(), dd.end());
        return dd[p * (dd.size() - 1)];
    }

    /// Scales the layout (i.e. modified \ref pointMap_) to the given dimensions.
    /// The new x-dimension of the drawing.
    /// The new y-dimension of the drawing.
    void scaleLayout_(int x, int y) {
        double minEdgeDistance = distancePercentile(0.1);
        double vertexSize = minEdgeDistance / 3;
        std::vector<double> xs, ys;
        for (auto el : pointMap_) {
            xs.push_back((el.second)[0]);
            ys.push_back((el.second)[1]);
        }
        double minX = *std::min_element(xs.begin(), xs.end()),
               maxX = *std::max_element(xs.begin(), xs.end()),
               minY = *std::min_element(ys.begin(), ys.end()),
               maxY = *std::max_element(ys.begin(), ys.end());

        // factors are computed as if currently the vertices ended
        // exactly at the borders
        double factorX = x / (maxX - minX + 2 * vertexSize);
        double factorY = y / (maxY - minY + 2 * vertexSize);
        double newVertexSize = vertexSize * std::min(factorX, factorY);
        factorX = (x - 2 * newVertexSize)/ (maxX - minX);
        factorY = (y - 2 * newVertexSize)/ (maxY - minY);

        for (auto vd : g_.vertexRange()) {
            pointMap_.at(vd)[0] =
                (pointMap_.at(vd)[0] - minX) * factorX + newVertexSize;
            pointMap_.at(vd)[1] =
                (pointMap_.at(vd)[1] - minY) * factorY + newVertexSize;
        }
        VertexStyle::sizeBase = newVertexSize;

        double newEdgeWidth = newVertexSize * 2 / 3;
        EdgeStyle::widthBase = newEdgeWidth;
    }

    /// Dumps the layout to stderr for debugging purposes.
    void dumpLayout() {
        std::cerr << "The Layout:" << std::endl;
        for (auto vd : make_iterator_range(vertices(g_)))
            std::cerr << g_[vd] << ":" << pointMap_.at(vd)[0] << " "
                      << pointMap_.at(vd)[1] << std::endl;
    }
};

#endif
