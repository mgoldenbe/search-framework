#ifndef DRAWER_H
#define DRAWER_H

#include "visual_log.h"

/// \name Checking whether the \c State has a function for computing layout.
/// @{
template <class State, typename = void_t<>>
struct has_layout : std::false_type {};

template <class State>
struct has_layout<State,
                  void_t<decltype(std::declval<State>().visualLocation(
                      std::declval<double &>(), std::declval<double &>()))>>
    : std::true_type {};

template <class State>
using HasLayout = typename std::enable_if<has_layout<State>::value>::type;

template <class State>
using HasNoLayout = typename std::enable_if<!has_layout<State>::value>::type;
/// @}

    // http://stackoverflow.com/a/33421942/2725810
    // http://tronche.com/gui/x/xlib/events/keyboard-pointer/keyboard-pointer.html#XButtonEvent
    // http://www.lemoda.net/c/xlib-resize/   // Draw only on Expose event!
    template <class Node>
    struct Drawer {
    using State = typename Node::State;
    using Graph = StateGraph<State>;
    using MyVisualLog = VisualLog<Node>;
    using VertexDescriptor = typename Graph::VertexDescriptor;
    using Point = typename Graph::Point;
    using PointMap = typename Graph::PointMap;

    // MyVisualLog is not const, since we will move in time...
    Drawer(Graph &g, MyVisualLog &log) : g_(g), log_(log) { changeLayout(); }

    const VertexDescriptor coordsToVD(double x, double y) const {
        for (auto &el: pointMap_) {
            auto vd = el.first;
            auto point = el.second;
            switch(log_.get(vd).shape) {
            case VertexShape::CIRCLE: {
                double myDistance = gu::distance({point[0], point[1]}, {x, y});
                if (myDistance <=
                    log_.get(vd).sizeFactor * VertexStyle::sizeBase)
                    return vd;
                break;
            }
            default: assert(0);
            }
        }
        return nullptr;
    }

    void changeLayout() {
        computePointMap();
        scaleLayout(sizex_, sizey_);
    }

    Graphics &graphics() {return graphics_;}

    void draw() {
        GroupLock lock{true, graphics_, true}; (void)lock;
        auto cr = graphics_.cr;
        cairo_set_source_rgb(cr, 56, 128, 4);

        cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
        for (auto vd : g_.vertexRange())
            drawVertex(vd, log_.get(vd));
        // Draws edges with default color first, so they are in the background
        for (int depth = 0; depth <= 1; ++depth) {
            for (auto from : g_.vertexRange()) {
                for (auto to : g_.adjacentVertexRange(from)) {
                    auto ed = g_.edge(from, to);
                    auto style = log_.get(ed);
                    if ((style.color != EdgeStyle::defaultColor) == depth)
                        drawEdge(from, to, log_.get(ed));
                }
            }
        }
    }

    void drawVertex(VertexDescriptor vd, const VertexStyle &style) {
        fillVertex(vd, style);
        emphasizeVertex(vd, style);
    }

    void drawVertex(VertexDescriptor vd) {
        drawVertex(vd, log_.get(vd));
    }

    void drawEdge(VertexDescriptor from, VertexDescriptor to,
                  const EdgeStyle &style) {
        auto cr = graphics_.cr;
        Color ec = style.color;
        if (ec == Color::NOVAL) return;
        cairo_set_source_rgb(cr, RGB::red(ec), RGB::green(ec), RGB::blue(ec));
        cairo_set_line_width(cr, style.widthFactor * EdgeStyle::widthBase);
        gu::Circle cFrom{gu::Point{pointMap_[from][0], pointMap_[from][1]},
                         VertexStyle::sizeBase};
        gu::Circle cTo{gu::Point{pointMap_[to][0], pointMap_[to][1]},
                       VertexStyle::sizeBase};
        connectCircles(cr, cFrom, cTo);
        if (style.arrow)
            inscribePolygon(cr, cFrom, 3, angle(cFrom.center, cTo.center), true);
    }

    int sizeX() { return sizex_; }
    int sizeY() { return sizey_; }
    void sizeX(int size) { sizex_ = size; }
    void sizeY(int size) { sizey_ = size; }

private:
    /// \name Computing the layout. Uses SFINAE to use the \c visualLocation
    /// function of State it has one or automatic layout otherwise. See
    /// \ref HasNoLayout<State> and HasLayout<State>.
    /// @{
    template <class State = State>
    HasNoLayout<State> computePointMap() {
        pointMap_ = g_.layout(true, true);
    }

    template <class State = State>
    HasLayout<State> computePointMap() {
        for (auto vd: g_.vertexRange()) {
            auto state = g_.state(vd);
            double x, y;
            state->visualLocation(x, y);
            Point myPoint;
            myPoint[0] = x; myPoint[1] = y;
            pointMap_[vd] = myPoint;
        }
    }
    /// @}

    void fillVertex(VertexDescriptor vd, const VertexStyle &style) {
        auto cr = graphics_.cr;
        Color fc = style.fillColor;
        if (fc == Color::NOVAL) return;
        cairo_set_source_rgb(cr, RGB::red(fc), RGB::green(fc), RGB::blue(fc));
        cairo_set_line_width(cr, 0.0);
        cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
        switch(style.shape) {
        case VertexShape::CIRCLE:
            cairo_arc(cr, pointMap_[vd][0], pointMap_[vd][1],
                      style.sizeFactor * VertexStyle::sizeBase, 0, 2 * M_PI);
            break;
        default: assert(0);
        }
        cairo_fill(cr);
        cairo_stroke(cr);
    }

    void emphasizeVertex(VertexDescriptor vd, const VertexStyle &style) {
        auto cr = graphics_.cr;
        Color ec = style.emphasisColor;
        if (ec == Color::NOVAL) return;
        cairo_set_source_rgb(cr, RGB::red(ec), RGB::green(ec), RGB::blue(ec));
        switch(style.shape) {
        case VertexShape::CIRCLE:
            cairo_arc(cr, pointMap_[vd][0], pointMap_[vd][1],
                      style.sizeFactor * (1.0 - style.emphasisSizeFactor) *
                          VertexStyle::sizeBase,
                      0, 2 * M_PI);
            break;
        default: assert(0);
        }
	cairo_fill(cr);
        cairo_stroke(cr);
    }

    double distancePercentile(double p) {
        std::vector<double> dd;
        for (auto ed: g_.edgeRange()) {
            auto from = g_.from(ed), to = g_.to(ed);
            auto fromPoint = pointMap_[from], toPoint = pointMap_[to];
            dd.push_back(gu::distance({fromPoint[0], fromPoint[1]},
                                      {toPoint[0], toPoint[1]}));
        }
        sort(dd.begin(), dd.end());
        assert(p >= 0 && p <= 1.0);
        return dd[p * (dd.size() - 1)];
    }

    void scaleLayout(int x, int y) {
        double minEdgeDistance = distancePercentile(0.1);
        double vertexSize = minEdgeDistance/3;
        std::vector<double> xs, ys;
        for (auto el: pointMap_) {
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
        double newEdgeWidth = newVertexSize * 2 / 3;

        for (auto vd : g_.vertexRange()) {
            pointMap_[vd][0] =
                (pointMap_[vd][0] + vertexSize - minX) * factorX;
            pointMap_[vd][1] =
                (pointMap_[vd][1] + vertexSize - minY) * factorY;
        }
        VertexStyle::sizeBase = newVertexSize;
        EdgeStyle::widthBase = newEdgeWidth;
    }

    void dumpLayout() {
        std::cout << "The Layout:" << std::endl;
        for (auto vd: make_iterator_range(vertices(g_)))
            std::cout << g_[vd] << ":" << pointMap_[vd][0] << " "
                      << pointMap_[vd][1] << std::endl;
    }

private:
    Graph &g_;
    MyVisualLog &log_;
    PointMap pointMap_;

    int sizex_ = 500;
    int sizey_ = 500;
    Graphics graphics_;
};

#endif
