#ifndef FORM_H
#define FORM_H

// Changing cursor:
// http://stackoverflow.com/q/21102958/2725810

int keycodeToAscii(Display *display, int keystate, int keycode) {
    // http://stackoverflow.com/a/22418839/2725810
    return XkbKeycodeToKeysym(display, keycode, 0,
                              keystate & ShiftMask ? 1 : 0);
}

struct EditField {
    EditField(Display *display, WINDOW *host, int beginRow, int beginCol,
              int nrows, int ncols, std::string label)
        : display_(display), host_(host), beginRow_(beginRow),
          beginCol_(beginCol), nrows_(nrows), ncols_(ncols), label_(label) {
        me_ = subpad(host, nrows, ncols, beginRow, beginCol);
        maxpos_ = nrows * ncols - 1;
    }

    //~EditField() { delwin(me_); }

    const std::string &get() const { return s_; }

    void set(const std::string &s) {
        s_ = s;
        cursor_ = s.size();
    }

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
    Display *display_;
    WINDOW *host_;
    WINDOW *me_;
    int beginRow_, beginCol_, nrows_, ncols_;
    std::string label_;
    int maxpos_;
    std::string s_{};
    int cursor_{};
};

struct Form {
    bool empty() const {return fields_.size() == 0;}

    // Returns true if the key has been handled by the form
    bool handle(int keystate, int keycode) {
        if (keystate == 23) { // Tab
            active_ =
                (active_ + (keystate & ShiftMask ? -1 : 1)) % fields_.size();
            return true;
        }
        return fields_[active_].handle(keystate, keycode);
    }

    void display() const {
        for (auto &f: fields_) f.display();
    }

    const std::string &get(int fieldN) const { return fields_[fieldN].get(); }

    void addField(const EditField &f) { fields_.push_back(f); }

    void set(const std::string &s) { fields_[active_].set(s); }

private:
    std::vector<EditField> fields_;
    int active_{};
};

#endif
