#ifndef TYPIST
#define TYPIST

#include <curses.h>
#include <sstream>
#include "MenuUtilities.h"

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

        messagesPad_ = newpad(1000, 400); // enough for sure

        commandsPad_ = newpad(nCommandRows_, 80);
        keypad(commandsPad_, TRUE);

        show();
    }

    ~Typist() {
        delwin(eventsPad_);
        endwin();
    }

    void show() const {
        getmaxyx(stdscr, maxRow_, maxColumn_);
        showLog();
        showMessages();
        showCommands();
    }

    WINDOW *commandsPad() { return commandsPad_; }

    void setStep(int step) {
        deactivateRow(step_);
        activateRow(step);
        step_ = step;
        show();
    }

    void message(std::string message) {
        wprintw(messagesPad_, "%s\n", message.c_str());
        nMessages_++;
    }

    void setMenu(MENU *menu) { menu_ = menu; show(); }

private:
    const AlgorithmLog &log_;
    WINDOW *titlePad_;
    WINDOW *eventsPad_;
    int step_; // active row
    int prefix_ = 5;
    int suffix_ = 5;

    WINDOW *messagesPad_;
    int nShownMessages_ = 5;
    int messagesBegin_ = prefix_ + suffix_ + 3;

    WINDOW *commandsPad_;
    int commandsBegin_ = messagesBegin_ + nShownMessages_ + 2;
    int nCommandRows_ = 10;
    MENU *menu_ = nullptr;

    int nMessages_ = 0;
    mutable int maxRow_, maxColumn_;

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

    void showLog() const {
        // int prefresh(WINDOW * pad, int pminrow, int pmincol, int sminrow,
        //              int smincol, int smaxrow, int smaxcol);
        //    pminrow and pmincol -- scroll values
        //    sminrow,  smincol,  smaxrow, smaxcol -- non-hidden area
        prefresh(titlePad_, 0, 0, 0, 0, 1, maxColumn_ - 1);
        int vScroll = std::max(0, step_ - prefix_);
        prefresh(eventsPad_, vScroll, 0, 1, 0, prefix_ + suffix_ + 1,
                 maxColumn_ - 1);
    }

    void makeTitle(int row, const std::string &title) const {
        int hlineLength = (maxColumn_ - 1 - title.size()) / 2;
        mvhline(row, 0, ACS_HLINE, hlineLength);
        mvprintw(row, hlineLength, title.c_str());
        mvhline(row, hlineLength + title.size(), ACS_HLINE, hlineLength);
        refresh();
    }

    void showMessages() const {
        makeTitle(messagesBegin_, " Messages ");
        prefresh(messagesPad_, nMessages_ - nShownMessages_, 0,
                 messagesBegin_ + 1, 0, messagesBegin_ + nShownMessages_,
                 maxColumn_ - 1);
    }

    void showCommands() const {
        makeTitle(commandsBegin_, " Commands ");
        if (menu_)
            prefresh(commandsPad_, 0, 0, commandsBegin_ + 1, 0,
                     std::min(commandsBegin_ + nCommandRows_, maxRow_ - 1),
                     maxColumn_ - 1);
    }
};

#endif
