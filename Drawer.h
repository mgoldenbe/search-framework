#ifndef DRAWER
#define DRAWER

// http://stackoverflow.com/a/33421942/2725810
// http://tronche.com/gui/x/xlib/events/keyboard-pointer/keyboard-pointer.html#XButtonEvent
// http://www.lemoda.net/c/xlib-resize/   // Draw only on Expose event!
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include "utilities.h"
#include "VisualizationUtilities.h"
#include "GeometryUtilities.h"

template <class Graph, class VisualLog, bool autoLayoutFlag>
struct Drawer {
    using VertexDescriptor = typename Graph::VertexDescriptor;
    using Point = typename Graph::Point;
    using PointMap = typename Graph::PointMap;

    // VisualLog is not const, since we will move in time...
    Drawer(Graph &g, VisualLog &log) : g_(g), log_(log) {
        /* Prepare the surface for drawer */
        display_ = XOpenDisplay(NULL);
        if (display_ == NULL) {
            fprintf(stderr, "Failed to open display\n");
            exit(-1);
        }
        // create a new cairo surface in an x11 window as well as a cairo_t* to
        // draw on the x11 window with.
        surface_ = create_x11_surface(display_, w_, root_, &sizex_, &sizey_);
        cr_ = cairo_create(surface_);
        changeLayout();
    }

    // http://stackoverflow.com/a/19308254/2725810
    ~Drawer() {
        cairo_destroy(cr_);
        cairo_surface_destroy(surface_);
        XCloseDisplay(display_);
        cairo_debug_reset_static_data();
    }

    Display *display() {return display_;}
    Window &window() { return w_; }
    Window &root() { return root_; }

    const VertexDescriptor coordsToVD(double x, double y) const {
        // Commented out is code for debugging
        //double bestDistance = 9999999;
        //VertexDescriptor bestVD;
        for (auto &el: pointMap_) {
            auto vd = el.first;
            auto point = el.second;
            switch(log_.vertexStyle(vd).shape) {
            case VertexShape::CIRCLE: {
                double myDistance = distance(point[0], point[1], x, y);
                if (myDistance <=
                    log_.vertexStyle(vd).sizeFactor * VertexStyle::sizeBase)
                    return vd;
                // if (myDistance < bestDistance) {
                //     bestDistance = myDistance;
                //     bestVD = vd;
                // }
                break;
            }
            default: assert(0);
            }
        }
        // std::cout << "\nDistance: " << bestDistance << std::endl;
        // std::cout << "State: " << *(g_.state(bestVD)) << std::endl;
        return nullptr;
    }

    void changeLayout() {
        computePointMap(std::integral_constant<bool, autoLayoutFlag>{});
        scaleLayout(sizex_, sizey_);
    }

    cairo_surface_t *surface() {return surface_;}
    cairo_t *cr() {return cr_;}

    void draw() {
        cairo_push_group(cr_);

        // Clear the background
        cairo_set_source_rgb(cr_, 0, 0, 0);
        cairo_paint(cr_);

        cairo_set_source_rgb(cr_, 56, 128, 4);
        for (int depth = 0; depth <= EdgeStyle::maxDepth; ++depth) {
            for (auto from : g_.vertexRange()) {
                for (auto to : g_.adjacentVertexRange(from)) {
                    auto ed = g_.edge(from, to);
                    auto style = log_.edgeStyle(ed);
                    if (style.depth == depth)
                        drawEdge(from, to, log_.edgeStyle(ed));
                }
            }
        }
        for (auto vd : g_.vertexRange())
            drawVertex(vd, log_.vertexStyle(vd));


        cairo_pop_group_to_source(cr_);
        cairo_paint(cr_);
        cairo_surface_flush(surface_);
        XFlush(display_);
    }

    int sizeX() { return sizex_; }
    int sizeY() { return sizey_; }
    void sizeX(int size) { sizex_ = size; }
    void sizeY(int size) { sizey_ = size; }

private:
    void computePointMap(std::true_type) {
        pointMap_ = g_.layout(true, true);
    }

    void computePointMap(std::false_type) {
        for (auto vd: g_.vertexRange()) {
            auto state = g_.state(vd);
            double x, y;
            state->visualLocation(x, y);
            Point myPoint;
            myPoint[0] = x; myPoint[1] = y;
            pointMap_[vd] = myPoint;
        }
    }

    void fillVertex(VertexDescriptor vd, const VertexStyle &style) {
        Color fc = style.fillColor;
        if (fc == Color::NOVAL) return;
        cairo_set_source_rgb(cr_, RGB::red(fc), RGB::green(fc), RGB::blue(fc));
        cairo_set_line_width(cr_, 1.0);
        switch(style.shape) {
        case VertexShape::CIRCLE:
            cairo_arc(cr_, pointMap_[vd][0], pointMap_[vd][1],
                      style.sizeFactor * VertexStyle::sizeBase, 0, 2 * M_PI);
            break;
        default: assert(0);
        }
        cairo_fill(cr_);
        cairo_stroke(cr_);
    }

    void emphasizeVertex(VertexDescriptor vd, const VertexStyle &style) {
        Color ec = style.emphasisColor;
        if (ec == Color::NOVAL) return;
        cairo_set_source_rgb(cr_, RGB::red(ec), RGB::green(ec), RGB::blue(ec));
        cairo_set_line_width(cr_,
                             style.emphasisWidthFactor * VertexStyle::sizeBase);
        switch(style.shape) {
        case VertexShape::CIRCLE:
            cairo_arc(cr_, pointMap_[vd][0], pointMap_[vd][1],
                      style.sizeFactor * (1.0 - style.emphasisWidthFactor) *
                          VertexStyle::sizeBase,
                      0, 2 * M_PI);
            break;
        default: assert(0);
        }
        cairo_stroke(cr_);
    }

    void drawVertex(VertexDescriptor vd, const VertexStyle &style) {
        fillVertex(vd, style);
        emphasizeVertex(vd, style);
    }

    void drawEdge(VertexDescriptor from, VertexDescriptor to,
                  const EdgeStyle &style) {
        Color ec = style.color;
        if (ec == Color::NOVAL) return;
        cairo_set_source_rgb(cr_, RGB::red(ec), RGB::green(ec), RGB::blue(ec));
        cairo_set_line_width(cr_, style.widthFactor * EdgeStyle::widthBase);
        double fromX = pointMap_[from][0], fromY = pointMap_[from][1];
        double toX = pointMap_[to][0], toY = pointMap_[to][1];
        cairo_move_to(cr_, fromX, fromY);
        cairo_line_to(cr_, toX, toY);
        cairo_stroke(cr_);
        if (style.arrow) {
            double radius = VertexStyle::sizeBase;
            cairo_save(cr_);
            cairo_translate(cr_, (fromX + toX) / 2, (fromY + toY) / 2);
            double angle = atan2(toY - fromY, toX - fromX);
            cairo_rotate(cr_, angle);
            double x = radius * (1 - cos(M_PI / 6));
            double y = radius * sin(M_PI / 6);
            cairo_move_to(cr_, radius, 0);
            //double d = distace(fromX, fromY, toX, toY);
            cairo_line_to(cr_, x, y);
            cairo_line_to(cr_, x, -y);
            cairo_line_to(cr_, radius, 0);
            cairo_stroke(cr_);
            cairo_translate(cr_, -(fromX + toX) / 2, -(fromY + toY) / 2);
            cairo_restore(cr_);
        }
    }

    double distancePercentile(double p) {
        std::vector<double> dd;
        for (auto ed: g_.edgeRange()) {
            auto from = g_.from(ed), to = g_.to(ed);
            auto fromPoint = pointMap_[from], toPoint = pointMap_[to];
            dd.push_back(
                distance(fromPoint[0], fromPoint[1], toPoint[0], toPoint[1]));
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
    VisualLog &log_;
    PointMap pointMap_;
    Display *display_;
    Window w_, root_;
    cairo_surface_t* surface_;
    cairo_t* cr_;
    int sizex_ = 500;
    int sizey_ = 500;
};

#endif
