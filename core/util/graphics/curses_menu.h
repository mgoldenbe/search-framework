#ifndef MENU_UTILITLIES_H
#define MENU_UTILITLIES_H

#include <curses.h>
#include <menu.h>

MENU *createMenu(WINDOW *w, std::vector<ITEM *> &items,
                 const std::vector<std::string> &choices,
                 const std::vector<std::string> &nonSelectable,
                 const std::vector<std::string> &selected,
                 int maxMenuRows,
                 bool multiFlag = false) {
    MENU *res;
    for (auto i = 0U; i < choices.size(); ++i) {
        auto item = new_item(choices[i].c_str(), choices[i].c_str());
        if (in(nonSelectable, choices[i])) item_opts_off(item, O_SELECTABLE);
        items.push_back(item);
    }
    items.push_back(nullptr);

    res = new_menu(items.data());
    if (multiFlag) menu_opts_off(res, O_ONEVALUE);
    menu_opts_off(res, O_SHOWDESC);

    set_menu_win(res, w);

    int maxRow, maxColumn;
    getmaxyx(stdscr, maxRow, maxColumn); (void)maxRow;
    set_menu_format(res, maxMenuRows,
                    maxColumn / (res->namelen + res->spc_cols));
    if (choices.size() > 1) set_menu_mark(res, "->");
    else set_menu_mark(res, "");

    // This is a magic sequence.
    // set_item_value() needs to be called after post_menu()...
    wclear(w);
    post_menu(res);
    for (auto i = 0U; i < choices.size(); ++i)
        if (in(selected, choices[i])) set_item_value(items[i], true);
    wrefresh(w);
    return res;
}

void destroyMenu(MENU *m) {
    if (!m) return;
    unpost_menu(m);
    for (auto i = 0U; i < (unsigned)m->nitems; ++i) free_item(menu_items(m)[i]);
    free_menu(m);
}

std::string menuChoice(MENU *m) {
    return item_name(current_item(m));
}

std::vector<std::string> menuChoices(MENU *m) {
    std::vector<std::string> res;
    for (auto i = 0U; i < (unsigned)m->nitems; ++i) {
        ITEM *item = menu_items(m)[i];
        if (item_value(item))
            res.push_back(item_name(item));
    }
    return res;
}

#endif
