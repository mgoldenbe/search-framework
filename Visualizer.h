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
#include "VisualizerMenus.h"

template <class Graph, class VisualLog>
struct Visualizer : VisualizerData<Graph, VisualLog> {
    using AlgorithmLog = typename VisualLog::AlgorithmLog;
    using AlgorithmEvent = typename AlgorithmLog::AlgorithmEvent;
    using Data = VisualizerData<Graph, VisualLog>;
    using typename Data::VISUALIZER_STATE;
    using Data::g_;
    using Data::log_;
    using Data::drawer_;
    using Data::typist_;
    using Data::s_;

    // VisualLog is not const, since we will move in time...
    Visualizer(const Graph &g, VisualLog &log) : Data(g, log), m_(*this) {}

    void run() {
        int iteration = 0;
        // msleep(10000);
        while (1) {
            msleep(1);
            if (!processEvents()) break;
            drawer_.draw();
            typist_.setStep(log_.step());
            if (s_ == VISUALIZER_STATE::PAUSE) {
                iteration = 0;
                continue;
            }
            if (s_ == VISUALIZER_STATE::GO)
                if (++iteration % (1000 / this->speed_) == 0) log_.next();
        }
    }

private:
    void scale(cairo_t *cr, double factor) {
        double centerXUser_before = sizex_ / 2, centerYUser_before = sizey_ / 2;
        cairo_device_to_user(cr, &centerXUser_before, &centerYUser_before);
        scale_ *= factor;
        cairo_scale(cr, factor, factor);
        double centerXDevice_after = centerXUser_before,
               centerYDevice_after = centerYUser_before;
        cairo_user_to_device(cr, &centerXDevice_after, &centerYDevice_after);
        cairo_translate(cr, (sizex_ / 2 - centerXDevice_after) / scale_,
                        (sizey_ / 2 - centerYDevice_after) / scale_);
    }

    void scaleUp(cairo_t *cr) { scale(cr, scaleStep_); }

    void scaleDown(cairo_t *cr) { scale(cr, 1.0 / scaleStep_); }

    // Returns true if need to continue or false if quiting
    bool processEvents() {
        XEvent e;
        cairo_surface_t *surface = drawer_.surface();
        cairo_t *cr = drawer_.cr();

        if (XPending(cairo_xlib_surface_get_display(surface))) {
            XNextEvent(cairo_xlib_surface_get_display(surface), &e);
            switch (e.type) {
            case KeyPress:
                switch (e.xkey.state) {
                case 4: // Ctrl is pressed
                    if (e.xkey.keycode == 21) {
                        scaleUp(cr);
                        break;
                    }
                    if (e.xkey.keycode == 20) {
                        scaleDown(cr);
                        break;
                    }
                    break;
                default:
                    switch (e.xkey.keycode) {
                    case 65: // space
                        menu_driver(m_.raw(), REQ_TOGGLE_ITEM);
                        break;
                    case 116: // Down
                        menu_driver(m_.raw(), REQ_DOWN_ITEM);
                        break;
                    case 111: // Up
                        menu_driver(m_.raw(), REQ_UP_ITEM);
                        break;
                    case 113: // Left
                        menu_driver(m_.raw(), REQ_LEFT_ITEM);
                        break;
                    case 114: // Right
                        menu_driver(m_.raw(), REQ_RIGHT_ITEM);
                        break;
                    case 117: // PageDown
                        menu_driver(m_.raw(), REQ_SCR_DPAGE);
                        break;
                    case 112: // PageUp
                        menu_driver(m_.raw(), REQ_SCR_UPAGE);
                        break;
                    case 36: // Enter
                        m_.handleEnter();
                        break;
                    case 9: // Esc
                        m_.handleEsc();
                        break;
                    default:
                        typist_.message("Unhandled keypress! State: " +
                                        str(e.xkey.state) + "  Code: " +
                                        str(e.xkey.keycode));
                    }
                }
                break;
            case ButtonPress:
                drag_start_x = e.xbutton.x;
                drag_start_y = e.xbutton.y;
                break;
            case ButtonRelease:
                if (last_delta_x == 0 && last_delta_y == 0) {
                    double x = e.xbutton.x, y = e.xbutton.y;
                    cairo_device_to_user(cr, &x, &y);
                    auto vd = this->drawer().coordsToVD(x, y);
                    if (vd) {
                        auto state = g_.state(vd);
                        this->searchFilter().filterState().set(state);
                        typist_.message("Set search filter: " + str(*state));
                    }
                }
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
            default:
                ;
                // typist_.message("Dropping unhandled");
                /*
                std::cout << "Dropping unhandled XEevent.type = " << e.type
                << "." << std::endl; */
            }
        }
        return true;
    }

private:
    AllMenus<Graph, VisualLog> m_;

    double sizex_ = 500.0;
    double sizey_ = 500.0;
    double scale_ = 1.0;
    double scaleStep_ = 1.5;
    int last_delta_x = 0, last_delta_y = 0;
    int drag_start_x, drag_start_y;
};

#endif
