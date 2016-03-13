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
#include <X11/XKBlib.h>
#include <X11/extensions/XTest.h>

#include "utilities.h"
#include "VisualizerMenus.h"

template <class Graph, class VisualLog, bool autoLayoutFlag>
struct Visualizer : VisualizerData<Graph, VisualLog, autoLayoutFlag> {
    using AlgorithmLog = typename VisualLog::AlgorithmLog;
    using AlgorithmEvent = typename AlgorithmLog::AlgorithmEvent;
    using Data = VisualizerData<Graph, VisualLog, autoLayoutFlag>;
    using typename Data::VISUALIZER_STATE;
    using Data::g_;
    using Data::log_;
    using Data::drawer_;
    using Data::typist_;
    using Data::s_;

    // VisualLog is not const, since we will move in time...
    Visualizer(Graph &g, VisualLog &log) : Data(g, log), m_(*this) {}

    void run() {
        int iteration = 0;
        // msleep(10000);
        while (1) {
            msleep(1);
            if (!processEvents()) break;
            if (drawFlag_) {
                drawer_.draw();
                drawFlag_ = false;
            }
            typist_.setStep(log_.step());
            typist_.show();
            if (s_ == VISUALIZER_STATE::PAUSE) {
                iteration = 0;
                continue;
            }
            if (s_ == VISUALIZER_STATE::GO)
                if (++iteration % (1000 / this->speed_) == 0) {
                    log_.next(drawer_);
                    //drawFlag_ = true;
                }
        }
    }

private:
    int &windowXSize() { return this->drawer_.graphics().windowXSize; }
    int &windowYSize() { return this->drawer_.graphics().windowYSize; }
    void scale(cairo_t *cr, double factor) {
        int sizex = windowXSize(), sizey = windowYSize();
        double centerXUser_before = sizex / 2, centerYUser_before = sizey / 2;
        cairo_device_to_user(cr, &centerXUser_before, &centerYUser_before);
        double &scale = drawer_.graphics().scale;
        scale *= factor;
        cairo_scale(cr, factor, factor);
        double centerXDevice_after = centerXUser_before,
               centerYDevice_after = centerYUser_before;
        cairo_user_to_device(cr, &centerXDevice_after, &centerYDevice_after);
        cairo_translate(cr, (sizex / 2 - centerXDevice_after) / scale,
                        (sizey / 2 - centerYDevice_after) / scale);

        this->drawer_.sizeX(this->drawer_.sizeX() * factor);
        this->drawer_.sizeY(this->drawer_.sizeY() * factor);
        updateSurfaceSize();
    }

    void updateSurfaceSize() {
        cairo_xlib_surface_set_size(
            this->drawer_.graphics().surface,
            std::max(this->drawer_.sizeX(), static_cast<int>(windowXSize())),
            std::max(this->drawer_.sizeY(), static_cast<int>(windowYSize())));
    }

    void scaleUp(cairo_t *cr) { scale(cr, scaleStep_); }

    void scaleDown(cairo_t *cr) { scale(cr, 1.0 / scaleStep_); }

    // Returns true if need to continue or false if quiting
    bool processEvents() {
        XEvent e;
        auto &graphics = drawer_.graphics();
        cairo_surface_t *surface = graphics.surface;
        cairo_t *cr = graphics.cr;
        int c;

        if ((c = getch()) != ERR) {
            this->typist_.message(
                "Console is active; "
                "only events in the X window are currently handled");
        }
        if (XPending(cairo_xlib_surface_get_display(surface))) {
            XNextEvent(cairo_xlib_surface_get_display(surface), &e);
            noEventCount_ = 0;
            if (e.type != MotionNotify) motionLast_ = false;
            switch (e.type) {
            case Expose:
                drawFlag_ = true;
                break;
            case KeyPress: {
                // this->typist_.message("Key event received!");
                auto &form = m_.curMenu()->form();
                if (!form.empty())
                    if (form.handle(e.xkey.state, e.xkey.keycode)) break;
            }
                switch (e.xkey.state) {
                case 4: // Ctrl is pressed
                    if (e.xkey.keycode == 21) {
                        scaleUp(cr);
                        drawFlag_ = true;
                        break;
                    }
                    if (e.xkey.keycode == 20) {
                        scaleDown(cr);
                        drawFlag_ = true;
                        break;
                    }
                    if (e.xkey.keycode == 113) { // Left
                        this->typist_.scrollLeft();
                        break;
                    }
                    if (e.xkey.keycode == 114) { // Right
                        this->typist_.scrollRight();
                        break;
                    }
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
                    {
                        m_.handleEnter();
                        break;
                    }
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
                buttonPressed_ = true;
                drag_start_x = e.xbutton.x;
                drag_start_y = e.xbutton.y;
                break;
            case ButtonRelease:
                buttonPressed_ = false;
                if (last_delta_x == 0 && last_delta_y == 0) {
                    double x = e.xbutton.x, y = e.xbutton.y;
                    cairo_device_to_user(cr, &x, &y);
                    auto vd = this->drawer().coordsToVD(x, y);
                    if (vd) {
                        auto state = g_.state(vd);
                        auto &form = this->m_.curForm();
                        if (!form.empty()) form.set(str(*state));
                    }
                }
                last_delta_x = 0;
                last_delta_y = 0;
                break;
            case MotionNotify:
                // http://cairographics.org/manual/cairo-Transformations.html#cairo-translate
                motionLast_ = true;
                if (!buttonPressed_) {
                    lastMotionX_ = e.xmotion.x;
                    lastMotionY_ = e.xmotion.y;
                } else {
                    PatternLock lock{drawer_.graphics()}; (void)lock;
                    double scale = drawer_.graphics().scale;
                    cairo_translate(
                        cr,
                        (e.xmotion.x - drag_start_x - last_delta_x) / scale,
                        (e.xmotion.y - drag_start_y - last_delta_y) / scale);
                    last_delta_x = e.xmotion.x - drag_start_x;
                    last_delta_y = e.xmotion.y - drag_start_y;
                }
                break;
            case ConfigureNotify: {
                windowXSize() = e.xconfigure.width;
                windowYSize() = e.xconfigure.height;
                updateSurfaceSize();
                drawFlag_ = true;
                break;
            }
            case ClientMessage:
                return false;
            default:
                ;
                // typist_.message("Dropping unhandled");
                /*
                std::cout << "Dropping unhandled XEevent.type = " << e.type
                << "." << std::endl; */
            }
        } else {
            noEventCount_++;
            if (motionLast_ && noEventCount_ > 100) {
                motionLast_ = false;
                double x = lastMotionX_, y = lastMotionY_;
                cairo_device_to_user(cr, &x, &y);
                auto vd = this->drawer().coordsToVD(x, y);
                if (vd) {
                    auto state = g_.state(vd);
                    typist_.message("Mouse over: " + str(*state));
                }
            }
        }

        return true;
    }

private:
    AllMenus<Graph, VisualLog, autoLayoutFlag> m_;

    double scaleStep_ = 1.5;
    int last_delta_x = 0, last_delta_y = 0;
    int drag_start_x, drag_start_y;
    bool buttonPressed_ = false;

    bool motionLast_ = false;
    int noEventCount_ = 0;
    int lastMotionX_, lastMotionY_;
    bool drawFlag_ = false;
};

#endif
