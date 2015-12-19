//#include <iostream>
//#include <unistd.h>
#include "ncurses.h"
int main() {
    int row,col;
    // http://www.tldp.org/HOWTO/NCURSES-Programming-HOWTO/attrib.html
    initscr();
    noecho();
    curs_set(0); // hide the cursor
    getmaxyx(stdscr, row, col);
    WINDOW *pad_ptr = newpad(row, col + 20);
    //clear();
    //move(0,0);
    refresh();
    wprintw(pad_ptr, "%s",
            "0aa0123456789b0123456789c0123456789d0123456789e0123456789\n");
    wprintw(pad_ptr, "%s",
            "1ab0123456789b0123456789c0123456789d0123456789e0123456789\n");
    wattron(pad_ptr, A_BOLD);
    wattron(pad_ptr, A_UNDERLINE);
    wprintw(pad_ptr, "%s",
            "2ac0123456789b0123456789c0123456789d0123456789e0123456789\n");
    wattroff(pad_ptr, A_BOLD);
    wattroff(pad_ptr, A_UNDERLINE);
    wprintw(pad_ptr, "%s",
            "3ad0123456789b0123456789c0123456789d0123456789e0123456789\n");
    wprintw(pad_ptr, "%s",
            "4ae0123456789b0123456789c0123456789d0123456789e0123456789\n");
    // int prefresh(WINDOW * pad, int pminrow, int pmincol, int sminrow,
    //              int smincol, int smaxrow, int smaxcol);
    //    pminrow and pmincol -- scroll values
    //    sminrow,  smincol,  smaxrow, smaxcol -- non-hidden area
    //refresh();
    prefresh(pad_ptr, 1, 15, 0, 0, 5, col-1);
    getch();
    wmove(pad_ptr, 1, 0);
    wprintw(pad_ptr, "%s",
            "1ab0123456789b0123456789c0123456789d0123456789e01234567890\n");
    prefresh(pad_ptr, 1, 15, 0, 0, 5, col-1);
    getch();
    delwin(pad_ptr);
    endwin();
    return 0;
}
