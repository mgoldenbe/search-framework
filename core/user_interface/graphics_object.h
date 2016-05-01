#ifndef GRAPHICS_OBJECT_H
#define GRAPHICS_OBJECT_H

struct Graphics {
    Graphics() {
        /* Prepare the surface for drawer */
        display = XOpenDisplay(NULL);
        if (display == NULL) {
            fprintf(stderr, "Failed to open display\n");
            exit(-1);
        }
        // create a new cairo surface in an x11 window as well as a cairo_t* to
        // draw on the x11 window with.
        surface =
            create_x11_surface(display, w, root, &windowXSize, &windowYSize);
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

    int windowXSize = 500, windowYSize = 500;
    double zeroX = 0.0, // beginning of drawing in device coordinates
           zeroY = 0.0; // at last re-drawing
    double margin = 1.0;
    double scale = 1.0;
};

struct PatternLock {
    PatternLock(Graphics &g) : g_(g) {
        p_ = cairo_get_source(g_.cr);
        cairo_pattern_reference(p_);
    }

    ~PatternLock() {
        cairo_set_source(g_.cr, p_); // this does not affect the reference count
        cairo_pattern_destroy(p_); // safe! can still paint!
        // std::cerr << cairo_pattern_get_reference_count (p_) << std::endl;
        cairo_paint(g_.cr);
    }

private:
    Graphics &g_;
    cairo_pattern_t *p_;
};

// translation since the last re-drawing
void deltaTranslate(Graphics &g, double &deltaX, double &deltaY,
                    bool absFlag = true) {
    double curZeroX = 0.0, curZeroY = 0.0;
    cairo_user_to_device(g.cr, &curZeroX, &curZeroY);
    if (absFlag) {
        deltaX = fabs(curZeroX - g.zeroX);
        deltaY = fabs(curZeroY - g.zeroY);
    }
    else {
        deltaX = curZeroX - g.zeroX;
        deltaY = curZeroY - g.zeroY;
    }
}
// Set origin to the current origin, so deltas are 0.
void resetOrigin(Graphics &g) {
    double curZeroX = 0.0, curZeroY = 0.0;
    cairo_user_to_device(g.cr, &curZeroX, &curZeroY);
    g.zeroX = curZeroX;
    g.zeroY = curZeroY;
}

double deltaTranslateX(Graphics &g) {
    double deltaX, deltaY;
    deltaTranslate(g, deltaX, deltaY);
    return deltaX;
}
double deltaTranslateY(Graphics &g) {
    double deltaX, deltaY;
    deltaTranslate(g, deltaX, deltaY);
    return deltaY;
}

bool redraw(Graphics &g) {
    double deltaX, deltaY;
    deltaTranslate(g, deltaX, deltaY);
    if (deltaX >= g.margin * g.windowXSize ||
        deltaY >= g.margin * g.windowYSize)
        return true;
    return false;
}

// Everything drawn between the execution of the constructor and the destructor
// is in a group. This drawing is appended to the current pattern.
struct GroupLock {
    GroupLock(bool flag, Graphics &g, bool clearFlag = false)
        : g_(g), flag_(flag) {
        if (!flag_) return;

        virtualTranslate(); // Translate to enable margins
                              // that are not clipped

        {
            PatternLock lock{g_};    // The constructor saves source as pattern.
                                     // The destructor will:
                                     //    1. Put the saved source
                                     //       as a source inside the group.
                                     //    2. Paint from the source into
                                     //       the temporary surface.
            cairo_push_group(g_.cr); // Re-direct all subsequent drawing into
                                     // temporary surface
            (void)lock;
        }

        if (clearFlag) {
            cairo_set_source_rgb(g_.cr, 0, 0, 0);
            cairo_paint(g_.cr);
        }
    }

    ~GroupLock() {
        if (!flag_) return;
        cairo_pop_group_to_source(g_.cr); // Get the image from the
                                          // temporary surface into the source.
        {
            PatternLock lock{g_};
            (void)lock;        // See above. Only painting
                               // into the real surface
            realTranslate(); // Translating it back
        }
    }

private:
    Graphics &g_;
    bool flag_;
    cairo_pattern_t *p_;
    double zeroX_, zeroY_;
    double origDeltaX_, origDeltaY_;

    void virtualTranslate() {
        deltaTranslate(g_, origDeltaX_, origDeltaY_, false);
        cairo_translate(g_.cr,
                        (-origDeltaX_ + g_.margin * g_.windowXSize) / g_.scale,
                        (-origDeltaY_ + g_.margin * g_.windowYSize) / g_.scale);
    }
    void realTranslate() {
        cairo_translate(g_.cr,
                        (origDeltaX_ - g_.margin * g_.windowXSize) / g_.scale,
                        (origDeltaY_ - g_.margin * g_.windowYSize) / g_.scale);
    }
};

#endif
