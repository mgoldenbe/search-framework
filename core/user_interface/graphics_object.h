#ifndef GRAPHICS_OBJECT_H
#define GRAPHICS_OBJECT_H

/// \file
/// \brief Facilities for manipulating the drawing surface.
/// \author Meir Goldenberg

/// Objects of this class hold all the information needed for drawing.
struct Graphics {
    /// Creates the window, initializes the surface and the drawing context.
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

    /// Frees the resources.
    ~Graphics() {
        // http://stackoverflow.com/a/19308254/2725810
        cairo_destroy(cr);
        cairo_surface_destroy(surface);
        XCloseDisplay(display);
        cairo_debug_reset_static_data();
    }

    /// \name The information needed for drawing.
    /// \brief Please refer to
    /// https://www.cairographics.org/Xlib/ for more information.
    /// @{
    Display *display{};
    Window w, root;
    cairo_surface_t* surface{};
    cairo_t* cr{};
    /// @}

    /// \name Window dimensions.
    /// @{
    int windowXSize = 500, windowYSize = 500;
    /// @}

    /// \name Top-left corner of the drawing.
    /// Device coordinates of the top-left corner of the drawing surface
    /// at the time of the last re-drawing.
    /// @{
        double zeroX = 0.0,
               zeroY = 0.0;
    /// @}

    double margin = 1.0; ///< The factor of window width/height added to the
                         ///drawing surface compared to the window size on each
                         ///side. For example, margin==1.0 means that the
                         ///dimensions of the drawing surface are three times
                         ///larger than the corresponding dimensions of the
                         ///window.

    double scale = 1.0; ///< Current scaling factor.
};

/// Implements a RAII technique to save and restore the current drawing. This
/// can be used, for example, to shift the whole drawing (all one needs to do is
/// to translate the drawing after acquiring a PatternLock).
struct PatternLock {
    /// Saves the current source as a pattern.
    PatternLock(Graphics &g) : g_(g) {
        p_ = cairo_get_source(g_.cr);
        cairo_pattern_reference(p_);
    }

    /// Restores the source from the pattern saved in the constructor.
    ~PatternLock() {
        cairo_set_source(g_.cr, p_); // this does not affect the reference count
        cairo_pattern_destroy(p_); // safe! can still paint!
        // std::cerr << cairo_pattern_get_reference_count (p_) << std::endl;
        cairo_paint(g_.cr);
    }

private:
    Graphics &g_; ///< The graphics object.
    cairo_pattern_t *p_; ///< The saved pattern.
};

/// Computes the drawing translation since the last re-drawing.
/// \param g The graphics object.
/// \param deltaX, deltaY The computed translation along the two dimensions.
/// \param absFlag If \c true, the translation along each dimension is computed
/// as an absolute value.
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

/// Sets \c g.zeroX and \c g.zeroY to the current device coordinates of the
/// top-left corner of the drawing (including the clipped part). After this,
/// \ref deltaTranslate computes the translation of zero along both dimensions.
/// \param g The graphics object.
void resetOrigin(Graphics &g) {
    double curZeroX = 0.0, curZeroY = 0.0;
    cairo_user_to_device(g.cr, &curZeroX, &curZeroY);
    g.zeroX = curZeroX;
    g.zeroY = curZeroY;
}

/// Computes the drawing translation along the X-dimension since the last
/// re-drawing.
/// \param g The graphics object.
/// \return The drawing translation along the X-dimension since the last
/// re-drawing.
double deltaTranslateX(Graphics &g) {
    double deltaX, deltaY;
    deltaTranslate(g, deltaX, deltaY);
    return deltaX;
}

/// Computes the drawing translation along the Y-dimension since the last
/// re-drawing.
/// \param g The graphics object.
/// \return The drawing translation along the Y-dimension since the last
/// re-drawing.
double deltaTranslateY(Graphics &g) {
    double deltaX, deltaY;
    deltaTranslate(g, deltaX, deltaY);
    return deltaY;
}

/// Determines whether the translation exceeds the margin, in which case some of the clipped off area will need to be loaded.
/// \param g The graphics object.
/// \return \c true if the translation exceeds the margin.
/// \see Graphics::margin
bool redraw(Graphics &g) {
    double deltaX, deltaY;
    deltaTranslate(g, deltaX, deltaY);
    if (deltaX >= g.margin * g.windowXSize ||
        deltaY >= g.margin * g.windowYSize)
        return true;
    return false;
}

/// A GroupLock object is created in order that the following drawing is done
/// in a group, possibly on top of the current drawing. \n
/// It cares for a lot of nitty-gritty details without which strange things
/// happen. \n
/// Uses \ref PatternLock to implement its
/// functionality.
/// \warning I got this right after a lot of trial and error. Do not try to
/// modify this unless you are a graphics person who knows what he is doing.
struct GroupLock {
    /// Saves the current drawing and starts a group for the drawing commands
    /// following the lock acquisition (i.e. creation of the GroupLock object).
    /// Clears the drawing if needed.
    /// \param flag If \c false, no lock is acquired. This enables a function
    /// where whether GroupLock should be acquired or not is a parameter.
    /// g The graphics object.
    /// \param clearFlag If \c true, the previous drawing needs is cleared.
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

    /// Appends the group the the current drawing.
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
    Graphics &g_; ///< The graphics object.
    bool flag_; ///< Indicates whether the lock should be acquired.
    /// \name Translation at the time of lock acquisition.
    /// @{
    double origDeltaX_, origDeltaY_; /// @}

    /// Translate the surface, so its top-left corner corresponds to the
    /// top-left corner of the window. This enables the margins.
    /// \see Graphics::margin
    void virtualTranslate() {
        deltaTranslate(g_, origDeltaX_, origDeltaY_, false);
        cairo_translate(g_.cr,
                        (-origDeltaX_ + g_.margin * g_.windowXSize) / g_.scale,
                        (-origDeltaY_ + g_.margin * g_.windowYSize) / g_.scale);
    }

    /// Restore the translation matrix that was modified by \ref
    /// virtualTranslate.
    void realTranslate() {
        cairo_translate(g_.cr,
                        (origDeltaX_ - g_.margin * g_.windowXSize) / g_.scale,
                        (origDeltaY_ - g_.margin * g_.windowYSize) / g_.scale);
    }
};

#endif
