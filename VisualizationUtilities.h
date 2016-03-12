#ifndef VISUALIZATION_UTILITIES
#define VISUALIZATION_UTILITIES

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <cairo-xlib.h>
#include "Colors.h"

cairo_surface_t *create_x11_surface(Display *, Window &, Window &, int *,
                                    int *);
struct Graphics {
    Graphics(int sizex, int sizey) {
        /* Prepare the surface for drawer */
        display = XOpenDisplay(NULL);
        if (display == NULL) {
            fprintf(stderr, "Failed to open display\n");
            exit(-1);
        }
        // create a new cairo surface in an x11 window as well as a cairo_t* to
        // draw on the x11 window with.
        surface = create_x11_surface(display, w, root, &sizex, &sizey);
        cr = cairo_create(surface);
    }

    ~Graphics() {
        // http://stackoverflow.com/a/19308254/2725810
        cairo_destroy(cr);
        cairo_surface_destroy(surface);
        XCloseDisplay(display);
        cairo_debug_reset_static_data();
    }
    Display *display{};
    Window w, root;
    cairo_surface_t* surface{};
    cairo_t* cr{};
};

struct GroupLock {
    GroupLock(bool flag, Graphics &g, bool clearFlag = false)
        : g_(g), flag_(flag) {
        if (!flag_) return;
        auto cr = g_.cr;
        cairo_push_group(cr);

        if (clearFlag) {
            cairo_set_source_rgb(cr, 0, 0, 0);
            cairo_paint(cr);
        }
    }

    ~GroupLock() {
        if (!flag_) return;
        cairo_pop_group_to_source(g_.cr);
        cairo_paint(g_.cr);
        cairo_surface_flush(g_.surface);
        XFlush(g_.display);
    }

private:
    Graphics &g_;
    bool flag_;
};


struct PatternLock {
    PatternLock(Graphics &g)
        : g_(g) {
        p_ = cairo_get_source(g_.cr);
        cairo_pattern_reference(p_);
    }

    ~PatternLock() {
        cairo_set_source_rgb(g_.cr, 0, 0, 0);
        cairo_paint(g_.cr);
        cairo_set_source(g_.cr, p_);
        cairo_paint(g_.cr);
        cairo_surface_flush(g_.surface);
        XFlush(g_.display);
        cairo_pattern_destroy(p_);
    }

private:
    Graphics &g_;
    cairo_pattern_t *p_;
};

struct RGB {
    static double red(Color color) {
        return (double)(RGBValues[static_cast<int>(color)] >> 16)/255;
    }
    static double green(Color color) {
        return (double)((RGBValues[static_cast<int>(color)] >> 8) % 256)/255;
    }
    static double blue(Color color) {
        return (double)(RGBValues[static_cast<int>(color)] % 256)/255;
    }
};

enum class VertexShape {CIRCLE, SQUARE};
enum class VertexFillPattern {SOLID};

enum class EdgeShape {CIRCLE, SQUARE};
enum class EdgeDash {NONE};

struct VertexStyle {
    VertexShape shape = VertexShape();
    VertexFillPattern pattern = VertexFillPattern();
    static double sizeBase;
    double sizeFactor = 1.0;
    Color fillColor = Color::WARM_GREY;
    Color borderColor  = Color();
    double borderWidthFactor = 0;
    Color emphasisColor = Color();
    double emphasisWidthFactor = 0;
};
double VertexStyle::sizeBase = 10;

struct EdgeStyle {
    bool operator==(const EdgeStyle &rhs) {
        if (shape != rhs.shape) return false;
        if (dash != rhs.dash) return false;
        if (color != rhs.color) return false;
        if (widthFactor != rhs.widthFactor) return false;
        return true;
    };
    EdgeShape shape = EdgeShape();
    EdgeDash dash = EdgeDash();
    Color color = Color::BROWN_GREY;
    static double widthBase;
    double widthFactor = 1.0;
    bool arrow = false;
    int depth = 0;
    static constexpr int maxDepth = 2;
};
double EdgeStyle::widthBase = 5.0;

//This function should give us a new x11 surface to draw on.
cairo_surface_t *create_x11_surface(Display *d, Window &w, Window &root, int *x,
                                    int *y) {
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
                            ButtonMotionMask | PointerMotionMask |
                            ExposureMask | StructureNotifyMask);

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

    root = XDefaultRootWindow(d);
    int revert;
    XGetInputFocus(d, &w, &revert);

    return sfc;
}

// Function to create an X11 keyboard event
// http://www.doctort.org/adam/nerd-notes/x11-fake-keypress-event.html
XKeyEvent createKeyEvent(Display *display, Window &win, Window &winRoot,
                         bool press, int keycode, int keystate) {
    XKeyEvent event;

    event.display     = display;
    event.window      = win;
    event.root        = winRoot;
    event.subwindow   = None;
    event.time        = CurrentTime;
    event.x           = 1;
    event.y           = 1;
    event.x_root      = 1;
    event.y_root      = 1;
    event.same_screen = True;
    event.keycode     = XKeysymToKeycode(display, keycode);
    event.state       = keystate;

    if(press)
        event.type = KeyPress;
    else
        event.type = KeyRelease;

    return event;
}


#endif
