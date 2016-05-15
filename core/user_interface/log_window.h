#ifndef TYPIST_H
#define TYPIST_H

#include "form.h"

template <class Node> struct LogWindow {
    using MyVisualLog = VisualLog<Node>;
    //using MyAlgorithmLog = AlgorithmLog<Node>;
    using State = typename Node::State;
    using AlgorithmEvent = typename Events::Base<Node>::Event;

    LogWindow(const MyVisualLog &log) : log_(log) {
        initscr();
        start_color();
        use_default_colors();
        init_pair(1, -1, -1);
        init_pair(2, COLOR_CYAN, -1);
        noecho();
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);
        curs_set(0);                // hide the cursor
        titlePad_ = newpad(1, 400); // enough for sure

        eventsPad_ = newpad(80, 400); // enough for sure

        messagesPad_ = newpad(1000, 400); // enough for sure

        commandsPad_ = newpad(nCommandRows_, 80);
        keypad(commandsPad_, TRUE);

        wbkgd(titlePad_, COLOR_PAIR(1));
        wbkgd(eventsPad_, COLOR_PAIR(1));
        wbkgd(messagesPad_, COLOR_PAIR(1));
        wbkgd(commandsPad_, COLOR_PAIR(1));
    }

    ~LogWindow() {
        delwin(eventsPad_);
        endwin();
    }

    void printEvent(int row) {
        int step = rowToStep(row);
        if (!log_.inFilter(step))
            wattron(eventsPad_, COLOR_PAIR(2));
        wprintw(eventsPad_, "%s\n", str(logTable_.row(row)).c_str());
        if (!log_.inFilter(step))
            wattroff(eventsPad_, COLOR_PAIR(2));
    }

    // Fill the currently visible area of eventsPad_
    void updateEventsPad() {
        wclear(eventsPad_);
        wrefresh(eventsPad_);
        int first = firstRow();
        for (int i = first;
             i <= std::min(first + prefix_ + suffix_, logTable_.size() - 1);
             i++)
            printEvent(i);
    }

    void fillEventsPad() {
        logTable_.clear();

        Events::Base<Node>::dumpTitle(logTable_);
        for (auto &e : log_.algorithmLog().events())
            if (!hideFiltered_ || log_.inFilter(e->step())) {
                e->dump(logTable_);
                logTable_ << std::endl;
            }

        wclear(titlePad_);
        wrefresh(titlePad_);
        wattron(titlePad_, A_BOLD);
        wprintw(titlePad_, str(logTable_.row(0)).c_str());
        wattroff(titlePad_, A_BOLD);
        logTable_.remove(0);

        updateEventsPad();
        activateRow(stepToEventPadRow(step_));
    }

    void show() {
        getmaxyx(stdscr, maxRow_, maxColumn_);
        showLog();
        showMessages();
        showCommands();
        refresh();
    }

    WINDOW *commandsPad() { return commandsPad_; }

    void setStep(int step) {
        deactivateRow(stepToEventPadRow(step_));
        activateRow(stepToEventPadRow(step));
        if (step != step_) {
            step_ = step;
            updateEventsPad();
        }
    }

    void message(std::string message) {
        wprintw(messagesPad_, "%s\n", message.c_str());
        nMessages_++;
    }

    void setMenu(MENU *menu, Form *form) {
        menu_ = menu;
        form_ = form;
    }

    void hideFiltered(bool flag) { hideFiltered_ = flag; }

    void scrollRight() { horizontalScroll_++; }
    void scrollLeft() {
        if (horizontalScroll_ > 0) horizontalScroll_--;
    }

private:
    const MyVisualLog &log_;
    Table logTable_{2};
    WINDOW *titlePad_;
    WINDOW *eventsPad_;
    int step_ = 0; // next step of the log
    int horizontalScroll_ = 0;

    int prefix_ = 5;
    int suffix_ = 5;

    WINDOW *messagesPad_;
    int nShownMessages_ = 5;
    int messagesBegin_ = prefix_ + suffix_ + 3;

    WINDOW *commandsPad_;
    int commandsBegin_ = messagesBegin_ + nShownMessages_ + 2;
    int nCommandRows_ = 10;
    MENU *menu_ = nullptr;
    Form *form_ = nullptr;
    bool hideFiltered_ = true;

    int nMessages_ = 0;
    mutable int maxRow_, maxColumn_;

    int stepToTableRow(int step) const {
        if (!hideFiltered_) return step;
        if (step == 0) return 0;
        return log_.stepToFiltered(step - 1) + 1;
    }

    int firstRow() const {
        int activeRow = stepToTableRow(step_);
        return std::max(0, activeRow - prefix_);
    }

    int stepToEventPadRow(int step) const {
        return stepToTableRow(step) - firstRow();
    }

    int padToTable(int row) const {
        return row + firstRow();
    }

    int rowToStep(int row) const {
        return hideFiltered_ ? log_.filteredToStep(row) : row;
    }

    void rowMode(int row, int attr, bool flag) {
        int tableRow = padToTable(row);
        int rowLimit = hideFiltered_ ? log_.nFilteredEvents() : log_.nEvents();
        if (tableRow < 0 || tableRow >= rowLimit) return;
        wmove(eventsPad_, row, 0);
        flag ? wattron(eventsPad_, attr) : wattroff(eventsPad_, attr);
        printEvent(tableRow);
        if (flag) wattroff(eventsPad_, attr);
    }

    void activateRow(int row) {
        rowMode(row - 1, A_UNDERLINE, true);
        rowMode(row, A_BOLD, true);
    }

    void deactivateRow(int row) {
        rowMode(row - 1, A_UNDERLINE, false);
        rowMode(row, A_BOLD, false);
    }

    void showLog() {
        // int prefresh(WINDOW * pad, int pminrow, int pmincol, int sminrow,
        //              int smincol, int smaxrow, int smaxcol);
        //    pminrow and pmincol -- scroll values
        //    sminrow,  smincol,  smaxrow, smaxcol -- non-hidden area
        prefresh(titlePad_, 0, horizontalScroll_, 0, 0, 1, maxColumn_ - 1);
        int vScroll = 0;//std::max(0, stepToRow(step_) - prefix_);
        prefresh(eventsPad_, vScroll, horizontalScroll_, 1, 0,
                 prefix_ + suffix_ + 1, maxColumn_ - 1);
    }

    void makeTitle(int row, const std::string &title) const {
        int hlineLength = (maxColumn_ - 1 - title.size()) / 2;
        mvhline(row, 0, ACS_HLINE, hlineLength);
        mvprintw(row, hlineLength, title.c_str());
        mvhline(row, hlineLength + title.size(), ACS_HLINE, hlineLength);
    }

    void showMessages() const {
        makeTitle(messagesBegin_, " Messages ");
        prefresh(messagesPad_, nMessages_ - nShownMessages_, 0,
                 messagesBegin_ + 1, 0, messagesBegin_ + nShownMessages_,
                 maxColumn_ - 1);
    }

    void showCommands() const {
        makeTitle(commandsBegin_, " Commands ");
        if (menu_ && form_ && !form_->empty())
            form_->display();
        prefresh(commandsPad_, 0, 0, commandsBegin_ + 1, 0,
                 std::min(commandsBegin_ + nCommandRows_, maxRow_ - 1),
                 maxColumn_ - 1);
    }
};

#endif
