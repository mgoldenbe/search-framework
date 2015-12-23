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

template <class Graph, class VisualLog> struct Visualizer {
    using AlgorithmLog = typename VisualLog::AlgorithmLog;
    using AlgorithmEvent = typename AlgorithmLog::AlgorithmEvent;
    enum class MENU_STATE {
        MAIN,
        RUN,
        SEARCH,
        FILTER_SHOW,
        FILTER_HIDE,
        GO,
        SPEED,
        TYPED_SEARCH,
        EDIT_FILTER
    };

    // VisualLog is not const, since we will move in time...
    Visualizer(const Graph &g, VisualLog &log)
        : log_(log), drawer_(g, log), typist_(log.algorithmLog()) {
        menuBackMap_ = {{MENU_STATE::MAIN, MENU_STATE::MAIN},
                        {MENU_STATE::RUN, MENU_STATE::MAIN},
                        {MENU_STATE::SEARCH, MENU_STATE::MAIN},
                        {MENU_STATE::FILTER_SHOW, MENU_STATE::MAIN},
                        {MENU_STATE::FILTER_HIDE, MENU_STATE::MAIN},
                        {MENU_STATE::GO, MENU_STATE::RUN},
                        {MENU_STATE::SPEED, MENU_STATE::RUN},
                        {MENU_STATE::SEARCH, MENU_STATE::TYPED_SEARCH},
                        {MENU_STATE::EDIT_FILTER, MENU_STATE::FILTER_SHOW}};
        menuMap_[MENU_STATE::MAIN] = {
            {"Run", MENU_STATE::RUN},
            {"Search", MENU_STATE::SEARCH},
            {"Filter",
             MENU_STATE::FILTER_SHOW}, // we'll switch to HIDE automatically in
                                       // the transition function when needed
            {"Layout", MENU_STATE::MAIN}};
        menuMap_[MENU_STATE::RUN] = {
            {"Go", MENU_STATE::GO},
            {"Speed (steps per sec.)", MENU_STATE::SPEED},
            {"Step Forward", MENU_STATE::RUN},
            {"Step Backward", MENU_STATE::RUN}};
        menuMap_[MENU_STATE::SEARCH] = {{"Event", MENU_STATE::TYPED_SEARCH},
                                        {"State", MENU_STATE::TYPED_SEARCH},
                                        {"Data", MENU_STATE::TYPED_SEARCH}};
        menuMap_[MENU_STATE::FILTER_SHOW] = {{"Edit", MENU_STATE::EDIT_FILTER},
                                             {"Show", MENU_STATE::FILTER_HIDE}};
        menuMap_[MENU_STATE::FILTER_HIDE] = {{"Edit", MENU_STATE::EDIT_FILTER},
                                             {"Show", MENU_STATE::FILTER_SHOW}};
        menuMap_[MENU_STATE::GO] = {{"Pause", MENU_STATE::RUN}};
        menuMap_[MENU_STATE::SPEED] = {{"1", MENU_STATE::RUN},
                                       {"2", MENU_STATE::RUN},
                                       {"5", MENU_STATE::RUN},
                                       {"10", MENU_STATE::RUN},
                                       {"50", MENU_STATE::RUN},
                                       {"200", MENU_STATE::RUN},
                                       {"500", MENU_STATE::RUN},
                                       {"1000", MENU_STATE::RUN}};
        menuMap_[MENU_STATE::TYPED_SEARCH] = {
            {"Forward", MENU_STATE::TYPED_SEARCH},
            {"Backward", MENU_STATE::TYPED_SEARCH}};
        menuMap_[MENU_STATE::EDIT_FILTER] = {
            {"All", MENU_STATE::FILTER_SHOW},
            {"None", MENU_STATE::FILTER_SHOW}};
        for (auto &el: AlgorithmEvent::eventTypeStr) {
            menuMap_[MENU_STATE::EDIT_FILTER].push_back(
                {el, MENU_STATE::FILTER_SHOW});
        }
        setMenuState(MENU_STATE::MAIN);
    }

    ~Visualizer() { destroyMenu(menu_); }

    void run() {
        int iteration = 0;
        // msleep(10000);
        while (1) {
            // std::cout << "Iteration " << iteration << std::endl;
            if (!processEvents()) break;
            drawer_.draw();
            typist_.setStep(log_.step());
            // typist_.type(log_.step());
            msleep(1);
            if (++iteration % 500 == 0) log_.next();
        }
    }

private:
    void setMenuState(MENU_STATE menuState, bool multiFlag = false) {
        destroyMenu(menu_);
        menuItems_.clear();
        std::vector<std::string> myChoices;
        std::transform(menuMap_[menuState].begin(),
                       menuMap_[menuState].end(),
                       std::back_inserter(myChoices),
                       [](const std::pair<std::string, MENU_STATE> &p) {
            return p.first;
        });
        menu_ = createMenu(typist_.commandsPad(), menuItems_,
                           myChoices, maxMenuRows_, multiFlag);
        menuState_ = menuState;
        typist_.setMenu(menu_);
    }

    void handleChoice() {
        for (auto &p: menuMap_[menuState_])
            if (p.first == menuChoice(menu_)) setMenuState(p.second);
    }

    void handleEsc() {
        setMenuState(menuBackMap_[menuState_]);
    }

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
                        menu_driver(menu_, REQ_TOGGLE_ITEM);
                        break;
                    case 116: // Down
                        menu_driver(menu_, REQ_DOWN_ITEM);
                        break;
                    case 111: // Up
                        menu_driver(menu_, REQ_UP_ITEM);
                        break;
                    case 113: // Left
                        menu_driver(menu_, REQ_LEFT_ITEM);
                        break;
                    case 114: // Right
                        menu_driver(menu_, REQ_RIGHT_ITEM);
                        break;
                    case 117: // PageDown
                        menu_driver(menu_, REQ_SCR_DPAGE);
                        break;
                    case 112: // PageUp
                        menu_driver(menu_, REQ_SCR_UPAGE);
                        break;
                    case 36: // Enter
                        handleChoice();
                        break;
                    case 9: // Esc
                        handleEsc();
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
    VisualLog &log_;
    Drawer<Graph, VisualLog> drawer_;
    Typist<AlgorithmLog> typist_;

    MENU *menu_ = nullptr;
    int maxMenuRows_ = 3;
    std::vector<ITEM *> menuItems_;
    std::map<MENU_STATE, std::vector<std::pair<std::string, MENU_STATE>>>
        menuMap_; // value -- for each choice, the following menuState
    std::map<MENU_STATE, MENU_STATE> menuBackMap_;
    MENU_STATE menuState_;

    double sizex_ = 500.0;
    double sizey_ = 500.0;
    double scale_ = 1.0;
    double scaleStep_ = 1.5;
    int last_delta_x = 0, last_delta_y = 0;
    int drag_start_x, drag_start_y;
};

#endif
