#ifndef DRAWER
#define DRAWER

// http://stackoverflow.com/a/33421942/2725810
// http://tronche.com/gui/x/xlib/events/keyboard-pointer/keyboard-pointer.html#XButtonEvent
// http://www.lemoda.net/c/xlib-resize/   // Draw only on Expose event!
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <unistd.h>
//#include <boost/thread/thread.hpp>
#include<time.h>
#include<signal.h>

#include "VisualizationUtilities.h"

// http://cc.byexamples.com/2007/05/25/nanosleep-is-better-than-sleep-and-usleep/
void __nsleep(const struct timespec *req, struct timespec *rem) {
    struct timespec temp_rem;
    if (nanosleep(req, rem) == -1)
        __nsleep(rem, &temp_rem);
}

int msleep(unsigned long milisec) {
    struct timespec req = {0,0}, rem = {0,0};
    time_t sec = (int)(milisec / 1000);
    milisec = milisec - (sec * 1000);
    req.tv_sec = sec;
    req.tv_nsec = milisec * 1000000L;
    __nsleep(&req, &rem);
    return 1;
}

//This function should give us a new x11 surface to draw on.
cairo_surface_t *create_x11_surface(Display *d, int *x, int *y) {
    Drawable da;
    int screen;
    cairo_surface_t *sfc;
    Screen *scr;

    screen = DefaultScreen(d);
    scr = DefaultScreenOfDisplay(d);
    if (!*x || !*y) {
        *x = WidthOfScreen(scr) / 2;
        *y = HeightOfScreen(scr) / 2;
        da =
            XCreateSimpleWindow(d, DefaultRootWindow(d), 0, 0, *x, *y, 0, 0, 0);
    } else
        da =
            XCreateSimpleWindow(d, DefaultRootWindow(d), 0, 0, *x, *y, 0, 0, 0);

    XSelectInput(d, da, ButtonPressMask | ButtonReleaseMask | KeyPressMask |
                            ButtonMotionMask | StructureNotifyMask);

    // http://www.lemoda.net/c/xlib-wmclose/index.html
    /* "wm_delete_window" is the Atom which corresponds to the delete
           window message sent by the window manager. */
    Atom wm_delete_window;
    wm_delete_window = XInternAtom(d, "WM_DELETE_WINDOW", False);
    /* Set up the window manager protocols. The third argument here is
       meant to be an array, and the fourth argument is the size of
       the array. */

    XSetWMProtocols(d, da, &wm_delete_window, 1);
    XMapWindow(d, da);

    sfc = cairo_xlib_surface_create(d, da, DefaultVisual(d, screen), *x, *y);

    return sfc;
}

template <class Graph, class VisualLog>
struct Drawer {
    using VertexDescriptor = typename Graph::VertexDescriptor;
    using Point = square_topology<>::point_type;
    using PointMap = std::map<VertexDescriptor, Point>;

    // VisualLog is not const, since we will move in time...
    Drawer(const Graph &g, VisualLog &log)
        : g_(g), log_(log), pointMap_(g.template layout<PointMap>()) {
        d = XOpenDisplay(NULL);
        if (d == NULL) {
            fprintf(stderr, "Failed to open display\n");
            exit(-1);
        }
        // create a new cairo surface in an x11 window as well as a cairo_t* to
        // draw on the x11 window with.
        int x=500, y=500;
        surface = create_x11_surface(d, &x, &y);
        cr = cairo_create(surface);

        scaleLayout(x, y, 20, 20);
    }

    // http://stackoverflow.com/a/19308254/2725810
    ~Drawer() {
        //std::cout << "In ~Drawer" << std::endl;
        cairo_destroy(cr);
        cairo_surface_destroy(surface);
        XCloseDisplay(d);
        cairo_debug_reset_static_data();
    }

    void run() {
        int iteration = 0;
        //msleep(10000);
        while (1) {
            // std::cout << "Iteration " << iteration << std::endl;
            if (!processEvents()) break;
            draw();
            // boost::this_thread::sleep( boost::posix_time::milliseconds(100));
            msleep(1);
            if (++iteration % 500 == 0) log_.next();
        }
    }

private:
    // Returns true if need to continue or false if quiting
    bool processEvents() {
        XEvent e;

        if (XPending(cairo_xlib_surface_get_display(surface))) {
            XNextEvent(cairo_xlib_surface_get_display(surface), &e);
            switch (e.type) {
            case ButtonPress:
                drag_start_x = e.xbutton.x;
                drag_start_y = e.xbutton.y;
                break;
            case ButtonRelease:
                last_delta_x = 0;
                last_delta_y = 0;
                break;
            case MotionNotify:
                // http://cairographics.org/manual/cairo-Transformations.html#cairo-translate
                cairo_translate(cr, e.xmotion.x - drag_start_x - last_delta_x,
                                e.xmotion.y - drag_start_y - last_delta_y);
                last_delta_x = e.xmotion.x - drag_start_x;
                last_delta_y = e.xmotion.y - drag_start_y;
                break;
            case ConfigureNotify:
                cairo_xlib_surface_set_size(surface, e.xconfigure.width,
                                            e.xconfigure.height);
                break;
            case ClientMessage:
                return false;
            default:
                fprintf(stderr, "Dropping unhandled XEevent.type = %d.\n",
                        e.type);
            }
        }
        return true;
    }


    void fillVertex(VertexDescriptor vd, const VertexStyle &style) {
        Color fc = style.fillColor;
        if (fc == Color::NOVAL) return;
        cairo_set_source_rgb(cr, RGB::red(fc), RGB::green(fc), RGB::blue(fc));
        cairo_set_line_width(cr, 1.0);
        switch(style.shape) {
        case VertexShape::CIRCLE:
            cairo_arc(cr, pointMap_[vd][0], pointMap_[vd][1], style.size, 0,
                      2 * M_PI);
            break;
        default: assert(0);
        }
        cairo_fill(cr);
        cairo_stroke(cr);
    }

    void emphasizeVertex(VertexDescriptor vd, const VertexStyle &style) {
        Color ec = style.emphasisColor;
        if (ec == Color::NOVAL) return;
        cairo_set_source_rgb(cr, RGB::red(ec), RGB::green(ec), RGB::blue(ec));
        cairo_set_line_width(cr, style.emphasisWidth);
        switch(style.shape) {
        case VertexShape::CIRCLE:
            cairo_arc(cr, pointMap_[vd][0], pointMap_[vd][1], style.size * 1.25,
                      0, 2 * M_PI);
            break;
        default: assert(0);
        }
        cairo_stroke(cr);
    }

    void drawVertex(VertexDescriptor vd, const VertexStyle &style) {
        fillVertex(vd, style);
        emphasizeVertex(vd, style);
    }

    void drawEdge(VertexDescriptor from, VertexDescriptor to,
                  const EdgeStyle &style) {
        Color ec = style.color;
        if (ec == Color::NOVAL) return;
        cairo_set_source_rgb(cr, RGB::red(ec), RGB::green(ec), RGB::blue(ec));
        cairo_set_line_width(cr, style.width);
        cairo_move_to(cr, pointMap_[from][0], pointMap_[from][1]);
        cairo_line_to(cr, pointMap_[to][0], pointMap_[to][1]);
        cairo_stroke(cr);
    }

    void draw() {
        cairo_push_group(cr);

        // Clear the background
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_paint(cr);

        cairo_set_source_rgb(cr, 56, 128, 4);
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


        cairo_pop_group_to_source(cr);
        cairo_paint(cr);
        cairo_surface_flush(surface);
        XFlush(d);
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
    Display *d;
    cairo_surface_t* surface;
    cairo_t* cr;
    int last_delta_x = 0, last_delta_y = 0;
    int drag_start_x, drag_start_y;
};

#endif
