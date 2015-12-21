#ifndef VISUALIZER
#define VISUALIZER

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
#include "Drawer.h"
#include "Typist.h"

template <class Graph, class VisualLog>
struct Visualizer {
    using AlgorithmLog = typename VisualLog::AlgorithmLog;

    // VisualLog is not const, since we will move in time...
    Visualizer(const Graph &g, VisualLog &log)
        : log_(log), drawer_(g, log), typist_(log.algorithmLog()) {}

    void run() {
        int iteration = 0;
        //msleep(10000);
        while (1) {
            // std::cout << "Iteration " << iteration << std::endl;
            if (!processEvents()) break;
            drawer_.draw();
            typist_.setStep(log_.step());
            //typist_.type(log_.step());
            msleep(1);
            if (++iteration % 500 == 0) log_.next();
        }
    }

private:
    void scale(cairo_t *cr, double factor) {
        double centerXUser_before = sizex_ / 2,
            centerYUser_before = sizey_ / 2;
        cairo_device_to_user(cr, &centerXUser_before,
                             &centerYUser_before);
        scale_ *= factor;
        cairo_scale(cr, factor, factor);
        double centerXDevice_after = centerXUser_before,
            centerYDevice_after = centerYUser_before;
        cairo_user_to_device(cr, &centerXDevice_after,
                             &centerYDevice_after);
        cairo_translate(cr, (sizex_ / 2 - centerXDevice_after) / scale_,
                        (sizey_ / 2 - centerYDevice_after) / scale_);
    }

    void scaleUp(cairo_t *cr) {
        scale(cr, scaleStep_);
    }

    void scaleDown(cairo_t *cr) {
        scale(cr, 1.0/scaleStep_);
    }

    // Returns true if need to continue or false if quiting
    bool processEvents() {
        XEvent e;
        cairo_surface_t *surface = drawer_.surface();
        cairo_t *cr = drawer_.cr();

        if (XPending(cairo_xlib_surface_get_display(surface))) {
            XNextEvent(cairo_xlib_surface_get_display(surface), &e);
            switch (e.type) {
            case KeyPress:
                if (e.xkey.state == 4)  { // Ctrl is pressed
                    if (e.xkey.keycode == 21)  {
                        scaleUp(cr);
                        break;
                    }
                    if (e.xkey.keycode == 20) {
                        scaleDown(cr);
                        break;
                    }
                }
                typist_.message("Unhandled keypress! State: " +
                                str(e.xkey.state) + "  Code: " +
                                str(e.xkey.keycode));
                break;
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
                cairo_translate(
                    cr, (e.xmotion.x - drag_start_x - last_delta_x) / scale_,
                    (e.xmotion.y - drag_start_y - last_delta_y) / scale_);
                last_delta_x = e.xmotion.x - drag_start_x;
                last_delta_y = e.xmotion.y - drag_start_y;
                break;
            case ConfigureNotify:
                sizex_ = e.xconfigure.width;
                sizey_ = e.xconfigure.height;
                cairo_xlib_surface_set_size(surface, sizex_, sizey_);
                break;
            case ClientMessage:
                return false;
            default: ;
                //typist_.message("Dropping unhandled");
                /*
                std::cout << "Dropping unhandled XEevent.type = " << e.type
                << "." << std::endl; */
            }
        }
        return true;
    }

private:
    VisualLog &log_;
    Drawer<Graph, VisualLog> drawer_;
    Typist<AlgorithmLog> typist_;
    double sizex_ = 500.0;
    double sizey_ = 500.0;
    double scale_ = 1.0;
    double scaleStep_ = 1.5;
    int last_delta_x = 0, last_delta_y = 0;
    int drag_start_x, drag_start_y;
};

#endif
