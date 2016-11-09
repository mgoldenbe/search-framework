#ifndef FORM_H
#define FORM_H

/// \file
/// \brief Implementation of a simple form for \c ncurses. Forms that come with
/// \c ncurses cannot be put in pad windows.
/// \author Meir Goldenberg

namespace slb {
namespace core {
namespace ui {

// Changing cursor:
// http://stackoverflow.com/q/21102958/2725810

/// Computes ascii code based on X11 keycode.
/// \param display Connection to X server.
/// \param keystate Key group.
/// \param keycode They key.
/// \return The ascii code corresponding to \c keystate and \c keycode.
/// \see http://www.dsm.fordham.edu/cgi-bin/man-cgi.pl?topic=XkbKeycodeToKeysym&ampsect=3
int keycodeToAscii(Display *display, int keystate, int keycode) {
    // http://stackoverflow.com/a/22418839/2725810
    return XkbKeycodeToKeysym(display, keycode, 0,
                              keystate & ShiftMask ? 1 : 0);
}

/// Implements a simple text edit field.
struct EditField {
    /// Initializes the text field.
    /// \param display Connection to X server.
    /// \param host Window identifier. In our case, the menu pad will be passed.
    /// \param beginRow Row position of the top-left corner of the form in \c
    /// host.
    /// \param beginCol Column position of the top-left corner of the form in \c
    /// host.
    /// \param nrows Number of rows in the edit field.
    /// \param ncols Number of columns in the edit field.
    /// \param label Text to be displayed next to the field.
    /// host.
    EditField(Display *display, WINDOW *host, int beginRow, int beginCol,
              int nrows, int ncols, std::string label)
        : display_(display), host_(host), beginRow_(beginRow),
          beginCol_(beginCol), nrows_(nrows), ncols_(ncols), label_(label) {
        me_ = subpad(host, nrows, ncols, beginRow, beginCol);
        maxpos_ = nrows * ncols - 1;
    }

    //~EditField() { delwin(me_); }

    /// Returns the contents of the field.
    /// \return The contents of the field.
    const std::string &get() const { return s_; }

    /// Sets the contents of the field.
    /// \param s New contents.
    void set(const std::string &s) {
        s_ = s;
        cursor_ = s.size();
    }

    /// Displays the field with the contents and the cursor.
    void display() const {
        wclear(me_);
        mvwprintw(me_, 0, 0, label_.c_str());
        mvwprintw(me_, 0, label_.size(), s_.c_str());

        // print cursor
        wattron(me_, A_UNDERLINE);
        mvwprintw(me_, 0, cursor_ + label_.size(),
                  cursor_ < (int)s_.size() ? std::string{s_[cursor_]}.c_str()
                                           : " ");
        wattroff(me_, A_UNDERLINE);
        //int pos = cursor_ + label_.size();
        //wmove(host_, beginRow_ + pos/ncols_, beginCol_ + pos % ncols_);
    }

    /// Changes the contents and the cursor position based on the given key that
    /// was pressed.
    /// \param keystate The key group.
    /// \param keycode The key.
    /// \return \c true if the key was handled by the edit field and \c false
    /// otherwise.
    bool handle(int keystate, int keycode) {
        switch (keycode) {
        case 110: // Home
            cursor_ = 0;
            break;
        case 115: // End
            cursor_ = s_.size();
            break;
        case 116: // Down
            cursor_ = std::min(cursor_ + ncols_, (int)s_.size());
            break;
        case 111: // Up
            cursor_ = std::max(cursor_ - ncols_, 0);
            break;
        case 113: // Left
            cursor_ = std::max(cursor_ - 1, 0);
            break;
        case 114: // Right
            cursor_ = std::min(cursor_ + 1, (int)s_.size());
            break;
        case 119: // Delete
            if (cursor_ < (int)s_.size()) s_.erase(cursor_, 1);
            break;
        case 22: // Backspace
            if (cursor_ > 0) s_.erase(cursor_-- - 1, 1);
            break;
        default:
            int ch = keycodeToAscii(display_, keystate, keycode);
            if (ch < 0x20 || ch > 0x7E) return false;
            s_.insert(cursor_, std::string{static_cast<char>(ch)});
            if (cursor_ == (int)s_.size() - 1) cursor_++;
        }
        return true;
    }

private:
    Display *display_; ///< Connection to X server.

    /// Window identifier. In our case, the menu pad will be passed.
    WINDOW *host_;

    /// The window identifier of the field. \c me_ is a sub-window of \c host_.
    WINDOW *me_;

    int beginRow_, ///< Row position of the top-left corner of the form in \c
                   /// host.
        beginCol_, ///< Column position of the top-left corner of the form in \c
                   /// host.
        nrows_,    ///< Number of rows in the edit field.
        ncols_;    ///< Number of columns in the edit field.
    std::string label_; ///< Text to be displayed next to the field.
    int maxpos_; ///< Maximal number of characters which the field can contain.
    std::string s_{}; ///< The contents of the field.
    int cursor_{};    ///< The cursor position.
};

/// A simple form for ncurses. Forms that come with \c ncurses cannot be put in
/// pad windows.
struct Form {
    /// Returns \c true if the form does not contain any fields and \c false
    /// otherwise.
    /// \return \c true if the form does not contain any fields and \c false
    /// otherwise.
    bool empty() const {return fields_.size() == 0;}

    /// Passes the given key that was pressed to the active field for handling.
    /// Handles by itself keys for moving between the fields.
    /// \param keystate The key group.
    /// \param keycode The key.
    /// \return \c true if the key was handled by the form and \c false
    /// otherwise.
    bool handle(int keystate, int keycode) {
        if (keystate == 23) { // Tab
            active_ =
                (active_ + (keystate & ShiftMask ? -1 : 1)) % fields_.size();
            return true;
        }
        return fields_[active_].handle(keystate, keycode);
    }

    /// Displays the form with all its fields.
    void display() const {
        for (auto &f: fields_) f.display();
    }

    /// Returns the contents of the active field.
    /// \return The contents of the active field.
    const std::string &get(int fieldN) const { return fields_[fieldN].get(); }

    /// Adds the given field to the form.
    /// \param f The field to be added.
    void addField(const EditField &f) { fields_.push_back(f); }

    /// Sets the contents of the active field.
    /// \param s New contents.
    void set(const std::string &s) { fields_[active_].set(s); }

private:
    std::vector<EditField> fields_; ///< Fields the constitute the form.
    int active_{}; ///< Index into \c fields_ indicating the active field.
};

} // namespace
} // namespace
} // namespace

#endif
