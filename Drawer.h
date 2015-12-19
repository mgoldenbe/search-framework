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

template <class Graph, class VisualLog>
struct Drawer {
    using VertexDescriptor = typename Graph::VertexDescriptor;
    using Point = square_topology<>::point_type;
    using PointMap = std::map<VertexDescriptor, Point>;

    // VisualLog is not const, since we will move in time...
    Drawer(const Graph &g, VisualLog &log)
        : g_(g), log_(log), pointMap_(g.template layout<PointMap>()) {
        /* Prepare the surface for drawer */
        display_ = XOpenDisplay(NULL);
        if (display_ == NULL) {
            fprintf(stderr, "Failed to open display\n");
            exit(-1);
        }
        // create a new cairo surface in an x11 window as well as a cairo_t* to
        // draw on the x11 window with.
        int x=500, y=500;
        surface_ = create_x11_surface(display_, &x, &y);
        cr_ = cairo_create(surface_);
        scaleLayout(x, y, 20, 20);
    }

    // http://stackoverflow.com/a/19308254/2725810
    ~Drawer() {
        cairo_destroy(cr_);
        cairo_surface_destroy(surface_);
        XCloseDisplay(display_);
        cairo_debug_reset_static_data();
    }

    const Display *display() const {return display_;}
    cairo_surface_t *surface() {return surface_;}
    cairo_t *cr() {return cr_;}

    void draw() {
        cairo_push_group(cr_);

        // Clear the background
        cairo_set_source_rgb(cr_, 0, 0, 0);
        cairo_paint(cr_);

        cairo_set_source_rgb(cr_, 56, 128, 4);
        for (int defaultFlag = 1; defaultFlag >= 0; --defaultFlag)
            for (auto from : g_.vertexRange()) {
                for (auto to : g_.adjacentVertexRange(from)) {
                    auto ed = g_.edge(from, to);
                    auto style = log_.edgeStyle(ed);
                    if ((style == VisualLog::VisualEvent::defaultEdgeStyle()) == defaultFlag)
                        drawEdge(from, to, log_.edgeStyle(ed));
                }
            }
        for (auto vd : g_.vertexRange())
            drawVertex(vd, log_.vertexStyle(vd));


        cairo_pop_group_to_source(cr_);
        cairo_paint(cr_);
        cairo_surface_flush(surface_);
        XFlush(display_);
    }

private:
    void fillVertex(VertexDescriptor vd, const VertexStyle &style) {
        Color fc = style.fillColor;
        if (fc == Color::NOVAL) return;
        cairo_set_source_rgb(cr_, RGB::red(fc), RGB::green(fc), RGB::blue(fc));
        cairo_set_line_width(cr_, 1.0);
        switch(style.shape) {
        case VertexShape::CIRCLE:
            cairo_arc(cr_, pointMap_[vd][0], pointMap_[vd][1], style.size, 0,
                      2 * M_PI);
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
        cairo_set_line_width(cr_, style.emphasisWidth);
        switch(style.shape) {
        case VertexShape::CIRCLE:
            cairo_arc(cr_, pointMap_[vd][0], pointMap_[vd][1], style.size * 1.25,
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
        cairo_set_line_width(cr_, style.width);
        cairo_move_to(cr_, pointMap_[from][0], pointMap_[from][1]);
        cairo_line_to(cr_, pointMap_[to][0], pointMap_[to][1]);
        cairo_stroke(cr_);
    }

    void scaleLayout(int x, int y, int marginX = 0, int marginY = 0) {
        std::vector<double> xs, ys;
        for (auto el: pointMap_) {
            xs.push_back((el.second)[0]);
            ys.push_back((el.second)[1]);
        }
        double minX = *std::min_element(xs.begin(), xs.end()),
               maxX = *std::max_element(xs.begin(), xs.end()),
               minY = *std::min_element(ys.begin(), ys.end()),
               maxY = *std::max_element(ys.begin(), ys.end());

        for (auto vd : g_.vertexRange()) {
            pointMap_[vd][0] =
                (pointMap_[vd][0] - minX) * (x - 2 * marginX) / (maxX - minX) +
                marginX;
            pointMap_[vd][1] =
                (pointMap_[vd][1] - minY) * (y - 2 * marginY) / (maxY - minY) +
                marginY;
        }
    }

    void dumpLayout() {
        std::cout << "The Layout:" << std::endl;
        for (auto vd: make_iterator_range(vertices(g_)))
            std::cout << g_[vd] << ":" << pointMap_[vd][0] << " "
                      << pointMap_[vd][1] << std::endl;
    }

private:
    const Graph &g_;
    VisualLog &log_;
    PointMap pointMap_;
    Display *display_;
    cairo_surface_t* surface_;
    cairo_t* cr_;
};

#endif
