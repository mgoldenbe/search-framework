#ifndef TYPIST
#define TYPIST

#include <curses.h>
#include <sstream>

template <class AlgorithmLog> struct Typist {
    Typist(const AlgorithmLog &log) : log_(log) {
        initscr();
        noecho();
        curs_set(0); // hide the cursor
        titlePad_ = newpad(1, 400); // enough for sure
        std::ostringstream ss;
        AlgorithmLog::AlgorithmEvent::dumpTitle(ss);
        wprintw(titlePad_, "%s\n", ss.str().c_str());
        eventsPad_ = newpad(80, 400); // enough for sure
        step_ = 0;

        // Fill the pad from the log
        for (auto &e : log_.events())
            wprintw(eventsPad_, "%s\n", str(e).c_str());
        activateRow(0);
        show();
    }

    ~Typist() {
        delwin(eventsPad_);
        endwin();
    }

    void setStep(int step) {
        deactivateRow(step_);
        activateRow(step);
        step_ = step;
        show();
    }
private:
    const AlgorithmLog &log_;
    WINDOW *titlePad_;
    WINDOW *eventsPad_;
    int step_; // active row
    int prefix_ = 5;
    int suffix_ = 5;

    void activateRow(int step) {
        wmove(eventsPad_, step, 0);
        wattron(eventsPad_, A_BOLD);
        wattron(eventsPad_, A_UNDERLINE);
        if (step < log_.events().size())
            wprintw(eventsPad_, "%s", str(log_.event(step)).c_str());
    }

    void deactivateRow(int step) {
        wmove(eventsPad_, step, 0);
        wattroff(eventsPad_, A_BOLD);
        wattroff(eventsPad_, A_UNDERLINE);
        if (step < log_.events().size())
            wprintw(eventsPad_, "%s", str(log_.event(step)).c_str());
    }

    void show() {
        int row, col;
        getmaxyx(stdscr, row, col);
        (void)row; (void)col;
        // int prefresh(WINDOW * pad, int pminrow, int pmincol, int sminrow,
        //              int smincol, int smaxrow, int smaxcol);
        //    pminrow and pmincol -- scroll values
        //    sminrow,  smincol,  smaxrow, smaxcol -- non-hidden area
        prefresh(titlePad_, 0, 0, 0, 0, 1, col - 1);
        int vScroll = std::max(0, step_ - prefix_);
        prefresh(eventsPad_, vScroll, 0, 1, 0, prefix_ + suffix_ + 1, col - 1);
    }
};

#endif
