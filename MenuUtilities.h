#ifndef MENU_UTILITLIES_H
#define MENU_UTILITLIES_H

#include <curses.h>
#include <menu.h>
#include <vector>
#include <string>

MENU *createMenu(WINDOW *w, std::vector<ITEM *> &items,
                 const std::vector<std::string> &choices,
                 int maxMenuRows,
                 bool multiFlag = false) {
    MENU *res;
    for (auto i = 0U; i < choices.size(); ++i)
        items.push_back(new_item(choices[i].c_str(), choices[i].c_str()));
    items.push_back(nullptr);

    res = new_menu(items.data());
    if (multiFlag) menu_opts_off(res, O_ONEVALUE);
    menu_opts_off(res, O_SHOWDESC);

    wclear(w);
    wrefresh(w);
    set_menu_win(res, w);

    int maxRow, maxColumn;
    getmaxyx(stdscr, maxRow, maxColumn); (void)maxRow;
    set_menu_format(res, maxMenuRows,
                    maxColumn / (res->namelen + res->spc_cols));
    //set_menu_spacing(res, 1, 2, 1);
    set_menu_mark(res, "->");

    post_menu(res);
    return res;
}

void destroyMenu(MENU *m) {
    if (!m) return;
    unpost_menu(m);
    free_menu(m);
    for (auto i = 0U; i < (unsigned)m->nitems; ++i) free_item(menu_items(m)[i]);
}

std::string menuChoice(MENU *m) {
    return item_name(current_item(m));
}

std::string menuChoices(MENU *m) {
    for (auto i = 0U; i < (unsigned)m->nitems; ++i) {
        ITEM *item = menu_items(m)[i];
        if (item_value(item))
            return item_name(item);
    }
    return "";
}

#endif
