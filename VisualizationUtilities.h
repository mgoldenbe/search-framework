#ifndef VISUALIZATION_UTILITIES
#define VISUALIZATION_UTILITIES

#include <cairo-xlib.h>
#include "Colors.h"

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
    VertexShape shape;
    VertexFillPattern pattern;
    int size;
    Color fillColor;
    Color borderColor;
    int borderWidth;
    Color emphasisColor;
    int emphasisWidth;
};

VertexStyle defaultVertexStyle() {
    VertexStyle res;
    res.shape = VertexShape();
    res.pattern = VertexFillPattern();
    res.size = 10;
    res.fillColor = Color::WARM_GREY;
    res.borderColor = Color();
    res.borderWidth = 0;
    res.emphasisColor = Color();
    res.emphasisWidth = 0;
    return res;
}

struct EdgeStyle {
    bool operator==(const EdgeStyle &rhs) {
        if (shape != rhs.shape) return false;
        if (dash != rhs.dash) return false;
        if (color != rhs.color) return false;
        if (width != rhs.width) return false;
        return true;
    };
    EdgeShape shape;
    EdgeDash dash;
    Color color;
    int width;
    bool arrow;
    int depth;
    static constexpr int maxDepth = 2;
};

EdgeStyle defaultEdgeStyle() {
    EdgeStyle res;
    res.shape = EdgeShape();
    res.dash = EdgeDash();
    res.color = Color::BROWN_GREY;
    res.width = 5;
    res.arrow = false;
    res.depth = 0;
    return res;
}

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
                            StructureNotifyMask);

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
