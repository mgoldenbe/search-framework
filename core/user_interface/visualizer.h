#ifndef VISUALIZER_H
#define VISUALIZER_H

/// \file
/// \brief The \ref ui::Visualizer class, which handles the user interface.
/// \author Meir Goldenberg

/// \namespace slb::core::ui
/// The user interface.
namespace ui {

using namespace util;

#include "drawer.h"
#include "filters.h"
#include "menus.h"

// http://stackoverflow.com/a/33421942/2725810
// http://tronche.com/gui/x/xlib/events/keyboard-pointer/keyboard-pointer.html#XButtonEvent
// http://www.lemoda.net/c/xlib-resize/   // Draw only on Expose event!

/// Handles the user interface.
/// \tparam Node The search node type.
template <class Node>
struct Visualizer : VisualizerData<Node> {
    /// The type of the (partial) domain graph.
    using Graph = StateGraph<typename Node::State>;

    /// The type of the log of events generated by the search algorithm.
    using MyAlgorithmLog = AlgorithmLog<Node>;

    /// The type of the log of visual events.
    using MyVisualLog = VisualLog<Node>;

    /// The type of the base holding the visualizer's constituent components.
    using Data = VisualizerData<Node>;

    /// Whether the visualizer is in paused or motion state.
    using typename Data::VISUALIZER_STATE;

    using Data::g_;         // ///< The (partial) domain graph.
    using Data::log_;       // ///< The log of visual events.
    using Data::drawer_;    // ///< The drawer object.
    using Data::logWindow_; // ///< The log window object.
    using Data::s_;         // ///< The current state of the visualizer.

    /// Initializes the visualizer with the given (partial) domain graph and log
    /// of visual events.
    /// \param g The (partial) domain graph.
    /// \param log The log of visual events.
    Visualizer(Graph &g, const MyAlgorithmLog &log)
        : Data(g, log), m_(*this) {}

    /// The event loop.
    void run() {
        int iteration = 0;
        int timer = 0; (void)timer;
        // msleep(10000);
        while (1) {
            msleep(1);
            // a cleaner way to do this:
            // http://stackoverflow.com/a/12871594/2725810
            if (drawFlag_ && ++timer % 250 == 0) {
                resetOrigin(drawer_.graphics());
                drawer_.draw();
                drawFlag_ = false;
                continue;
            }
            if (!processEvents()) break;
            logWindow_.setStep(log_.step());
            logWindow_.show();
            if (s_ == VISUALIZER_STATE::PAUSE) {
                iteration = 0;
                continue;
            }
            if (s_ == VISUALIZER_STATE::GO)
                if (++iteration % (1000 / this->speed_) == 0) {
                    if (!log_.next(drawer_))
                        s_ = VISUALIZER_STATE::PAUSE;
                    //drawFlag_ = true;
                }
        }
    }

private:
    /// Scale up to the next higher scale level.
    void scaleUp() { drawer_.graphics().scale(scaleStep_); }

    /// Scale down to the next lower scale level.
    void scaleDown() { drawer_.graphics().scale(1.0 / scaleStep_); }

    /// Processes pending key and mouse events.
    /// \return \c false if the user chose to quit the application and \c true
    /// otherwise.
    bool processEvents() {
        XEvent e;
        auto &graphics = drawer_.graphics();
        cairo_surface_t *surface = graphics.surface;
        cairo_t *cr = graphics.cr;
        int c;

        if ((c = getch()) != ERR) {
            /*
            this->logWindow_.message(
                "Console is active; "
                "only events in the X window are currently handled");
            */
        }
        if (XPending(cairo_xlib_surface_get_display(surface))) {
            XNextEvent(cairo_xlib_surface_get_display(surface), &e);
            noEventCount_ = 0;
            if (e.type != MotionNotify) motionLast_ = false;
            switch (e.type) {
            case Expose:
                drawFlag_ = true;
                // logWindow_.message("Expose event. Re-drawing...");
                break;
            case KeyPress: {
                // this->logWindow_.message("Key event received!");
                auto &form = m_.curMenu()->form();
                if (!form.empty())
                    if (form.handle(e.xkey.state, e.xkey.keycode)) break;
            }
                switch (e.xkey.state) {
                case 4: // Ctrl is pressed
                    if (e.xkey.keycode == 21) {
                        scaleUp();
                        drawFlag_ = true;
                        break;
                    }
                    if (e.xkey.keycode == 20) {
                        scaleDown();
                        drawFlag_ = true;
                        break;
                    }
                    if (e.xkey.keycode == 113) { // Left
                        this->logWindow_.scrollLeft();
                        break;
                    }
                    if (e.xkey.keycode == 114) { // Right
                        this->logWindow_.scrollRight();
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
                    default: break;
                        logWindow_.message("Unhandled keypress! State: " +
                                        str(e.xkey.state) + "  Code: " +
                                        str(e.xkey.keycode));
                    }
                }
                break;
            case ButtonPress:
                buttonPressed_ = true;
                drag_start_x = e.xbutton.x;
                drag_start_y = e.xbutton.y;
                last_delta_x = last_delta_y = 0;
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
                    if (redraw(graphics)) {
                        if (!drawFlag_) logWindow_.message("Loading...");
                        drawFlag_ = true;
                        drag_start_x = e.xmotion.x;
                        drag_start_y = e.xmotion.y;
                        last_delta_x = last_delta_y = 0;
                    } else {
                        // logWindow_.message("Translating...");
                        PatternLock lock{drawer_.graphics()};
                        (void)lock;
                        double scale = drawer_.graphics().scaleFactor;
                        cairo_translate(
                            cr,
                            (e.xmotion.x - drag_start_x - last_delta_x) / scale,
                            (e.xmotion.y - drag_start_y - last_delta_y) /
                                scale);
                        last_delta_x = e.xmotion.x - drag_start_x;
                        last_delta_y = e.xmotion.y - drag_start_y;
                    }
                }
                break;
            case ConfigureNotify: {
                drawer_.graphics().updateWindowSize(e.xconfigure.width,
                                                    e.xconfigure.height);
                //logWindow_.message("Configure event. Re-drawing...");
                //drawFlag_ = true;
                break;
            }
            case ClientMessage: // Window closed
                return false;
            default:
                ;
                // logWindow_.message("Dropping unhandled");
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
                    std::string eventStr;
                    try {
                        eventStr = "    Last event: " +
                                   this->log()
                                       .algorithmLog()
                                       .getLastEvent(state, this->log().step())
                                       ->eventStr();
                        (void)eventStr; // TODO Sometimes this is "Hide Last
                                        // Event", which is not very helpful;
                                        // need something better to display
                    } catch (...) {
                    }
                    logWindow_.message("Mouse over: " +
                                       str(*state) /*+ eventStr*/);
                }
            }
        }

        return true;
    }

private:
    AllMenus<Node> m_; ///< The menus.

    double scaleStep_ = 1.5; ///< The factor for a single scaling up or down.
    int last_delta_x = 0, ///< Translation of the drawing along the x-dimension
                          /// at the last re-drawing.
        last_delta_y = 0; ///< Translation of the drawing along the y-dimension
                          /// at the last re-drawing.
    int drag_start_x,     ///< x-position of the mouse at the beginning of the
                          /// current dragging.
        drag_start_y;     ///< y-position of the mouse at the beginning of the
                          /// current dragging.
    bool buttonPressed_ = false; ///< \c true if a mouse button has been pressed.

    bool motionLast_ = false; ///< \c true if the last event was mouse motion.

    /// Number of iterations of the events loop without an event. Used to
    /// determine mouse hovering over a vertex.
    int noEventCount_ = 0;

    int lastMotionX_, ///< x-coordinate of the location where the mouse was last
                      /// spotted moving without a button pressed. Used to
                      /// determine mouse hovering over a vertex.
        lastMotionY_; ///< y-coordinate of the location where the mouse was last
                      /// spotted moving without a button pressed. Used to
                      /// determine mouse hovering over a vertex.
    bool drawFlag_ = false; ///< \c True if re-drawing of the whole graphical
                            /// representation is required, e.g. upon scaling.
};

} // namespace

#endif
