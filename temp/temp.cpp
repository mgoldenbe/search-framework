#include "ncurses.h"
#include <cstdlib>

// http://stackoverflow.com/a/8192276/2725810

// See also this to handle highlighting:
// http://stackoverflow.com/a/6848670/2725810
MEVENT mev;

void quit(void)
{
    endwin();
}

int main(void)
{
    initscr();
    atexit(quit);
    clear();
    noecho();
    curs_set(0);
    cbreak();
    keypad(stdscr, TRUE);
    start_color();
    mousemask(BUTTON1_CLICKED, 0);

    mvaddstr(5, 3, "Click to turn a character into an 'X'");
    refresh();

    for(;;)
        {
            if(getch() == KEY_MOUSE && getmouse(&mev) == OK)
                {
                    mvaddch(mev.y,mev.x,'X');
                    refresh();
                }
        }
    return (0);
}
