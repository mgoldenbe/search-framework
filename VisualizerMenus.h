#ifndef VISUALIZER_MENUS_H
#define VISUALIZER_MENUS_H

#include <curses.h>
#include <menu.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "Drawer.h"
#include "Typist.h"
#include "Filter.h"

template <class Graph, class VisualLog> struct VisualizerData  {
    using AlgorithmLog = typename VisualLog::AlgorithmLog;
    using AlgorithmEvent = typename AlgorithmLog::AlgorithmEvent;
    enum class VISUALIZER_STATE{PAUSE, GO};

    VisualizerData(const Graph &g, VisualLog &log)
        : g_(g), log_(log), drawer_(g, log), typist_(log) {
        typist_.fillEventsPad();
    }
    VisualLog &log() { return log_; }
    Drawer<Graph, VisualLog> &drawer() { return drawer_; }
    Typist<VisualLog> &typist() { return typist_; }
    void state(VISUALIZER_STATE s) { s_ = s; }
    void speed(int s) { speed_ = s; }
    Filter<AlgorithmEvent> &filter() { return filter_; }
    Filter<AlgorithmEvent> &searchFilter() { return searchFilter_; }

protected:
    const Graph &g_;
    VisualLog &log_;
    Drawer<Graph, VisualLog> drawer_;
    Typist<VisualLog> typist_;

    VISUALIZER_STATE s_ = VISUALIZER_STATE::PAUSE;
    int speed_ = 2;
    Filter<AlgorithmEvent> filter_;
    Filter<AlgorithmEvent> searchFilter_;
};

template <class AllMenus, class Graph, class VisualLog>
struct MenuBase {
    using Base = MenuBase<AllMenus, Graph, VisualLog>;
    using Data = VisualizerData<Graph, VisualLog>;

    MenuBase(AllMenus &m, Data &data)
        : m_(m), data_(data) {}

    const std::vector<std::string> &choices() const { return choices_; }

    const std::vector<std::string> &nonSelectable() const {
        return nonSelectable_;
    }

    virtual bool multi() const { return false; }

    Data &data() { return data_; }

    virtual void handleEnter() {
        std::string choice = this->choice();
        Base *m = std::find_if(enterMap_.begin(), enterMap_.end(),
                               [choice](std::pair<std::string, Base *> &p) {
            return p.first == choice;
        })->second;
        m_.setMenu(m, data_.typist());
    }

    void handleEsc() { m_.setMenu(exitMenu_, data_.typist()); }

    void selectAll() {
        for (auto item: m_.menuItems()) set_item_value(item, true);
    }

    void selectNone() {
        for (auto item: m_.menuItems()) set_item_value(item, false);
    }

protected:
    AllMenus &m_;
    Data &data_;
    std::vector<std::pair<std::string, Base *>> enterMap_;
    std::vector<std::string> choices_;
    std::vector<std::string> nonSelectable_;
    Base *exitMenu_;

    void fillChoices() {
        choices_.clear();
        std::transform(
            enterMap_.begin(), enterMap_.end(), std::back_inserter(choices_),
            [](const std::pair<std::string, Base *> &p) { return p.first; });
    }

    std::string choice() const { return menuChoice(m_.raw()); }
};

template <class AllMenus, class Graph, class VisualLog>
struct MenuMain : public MenuBase<AllMenus, Graph, VisualLog> {
    MenuMain(AllMenus &m, VisualizerData<Graph, VisualLog> &data)
        : MenuBase<AllMenus, Graph, VisualLog>(m, data) {
        this->enterMap_ = {{"Run", &m.menuRun},
                           {"Search", &m.menuTypedSearch},
                           {"Filter", &m.menuFilter},
                           {"Layout", &m.menuMain}};
        this->fillChoices();
        this->exitMenu_ = &m.menuMain;
    }
};

template <class AllMenus, class Graph, class VisualLog>
struct MenuRun : MenuBase<AllMenus, Graph, VisualLog> {
    using Base = MenuBase<AllMenus, Graph, VisualLog>;
    using Data = typename Base::Data;

    MenuRun(AllMenus &m, Data &data) : Base(m, data) {
        this->enterMap_ = {{"Go", &m.menuGo},
                           {"Speed (steps/sec.)", &m.menuSpeed},
                           {"Step Forward", &m.menuRun},
                           {"Step Backward", &m.menuRun},
                           {"Reset", &m.menuRun}};
        this->fillChoices();
        this->exitMenu_ = &m.menuMain;
    }

    virtual void handleEnter() {
        std::string choice = this->choice();
        if (choice == "Go") this->data_.state(Data::VISUALIZER_STATE::GO);
        if (choice == "Step Forward") this->data_.log().next();
        if (choice == "Step Backward") this->data_.log().prev();
        if (choice == "Reset") this->data_.log().reset();
        Base::handleEnter();
    }
};

template <class AllMenus, class Graph, class VisualLog>
struct MenuSearch : MenuBase<AllMenus, Graph, VisualLog> {
    MenuSearch(AllMenus &m, VisualizerData<Graph, VisualLog> &data)
        : MenuBase<AllMenus, Graph, VisualLog>(m, data) {
        this->enterMap_ = {{"By event", &m.menuTypedSearch},
                           {"By state", &m.menuTypedSearch},
                           {"By data", &m.menuTypedSearch}};
        this->fillChoices();
        this->exitMenu_ = &m.menuMain;
    }
};

template <class AllMenus, class Graph, class VisualLog>
struct MenuFilter : MenuBase<AllMenus, Graph, VisualLog> {
    using Base = MenuBase<AllMenus, Graph, VisualLog>;
    using Data = typename Base::Data;

    MenuFilter(AllMenus &m, VisualizerData<Graph, VisualLog> &data)
        : MenuBase<AllMenus, Graph, VisualLog>(m, data) {
        this->enterMap_ = {{"Edit", &m.menuEditFilter},
                           {m.hideFiltered ? "Show" : "Hide", &m.menuFilter}};
        this->fillChoices();
        this->exitMenu_ = &m.menuMain;
    }

    virtual void handleEnter() {
        std::string choice = this->choice();
        if (choice == "Show" || choice == "Hide") {
            this->m_.hideFiltered = !this->m_.hideFiltered;
            std::string updatedItem = this->m_.hideFiltered ? "Show" : "Hide";

            this->enterMap_[1].first = updatedItem;
            // Has to be changed at the low level as well, so the matching in
            // Base::handleEnter() would succeed.
            current_item(this->m_.raw())->name.str = updatedItem.c_str();
            current_item(this->m_.raw())->name.length = updatedItem.size();

            this->data_.typist().hideFiltered(this->m_.hideFiltered);
            this->data_.typist().fillEventsPad();

            // Cause the item to update
            menu_driver(this->m_.raw(), REQ_LEFT_ITEM);
            menu_driver(this->m_.raw(), REQ_RIGHT_ITEM);
        }

        Base::handleEnter(); // Need to do this before the menu entry changed
    }
};

template <class AllMenus, class Graph, class VisualLog>
struct MenuGo : MenuBase<AllMenus, Graph, VisualLog> {
    using Base = MenuBase<AllMenus, Graph, VisualLog>;
    using Data = typename Base::Data;
    MenuGo(AllMenus &m, Data &data) : Base(m, data) {
        this->enterMap_ = {{"Pause", &m.menuRun}};
        this->fillChoices();
        this->exitMenu_ = &m.menuMain;
    }

    virtual void handleEnter() {
        this->data_.state(Data::VISUALIZER_STATE::PAUSE);
        Base::handleEnter();
    }
};

template <class AllMenus, class Graph, class VisualLog>
struct MenuSpeed : MenuBase<AllMenus, Graph, VisualLog> {
    using Base = MenuBase<AllMenus, Graph, VisualLog>;
    using Data = typename Base::Data;

    MenuSpeed(AllMenus &m, Data &data) : Base(m, data) {
        for (auto el : std::vector<std::string>{"1", "2", "5", "10", "50",
                                                "200", "500", "1000"})
            this->enterMap_.push_back({el, &m.menuRun});

        this->fillChoices();
        this->exitMenu_ = &m.menuRun;
    }

    virtual void handleEnter() {
        int speed = boost::lexical_cast<int>(this->choice());
        this->data_.speed(speed);
        Base::handleEnter();
    }
};

template <class AllMenus, class Graph, class VisualLog>
struct MenuTypedSearch : MenuBase<AllMenus, Graph, VisualLog> {
    using Base = MenuBase<AllMenus, Graph, VisualLog>;
    using Data = typename Base::Data;

    MenuTypedSearch(AllMenus &m, VisualizerData<Graph, VisualLog> &data)
        : MenuBase<AllMenus, Graph, VisualLog>(m, data) {
        this->enterMap_ = {{"Forward", &m.menuTypedSearch},
                           {"Backward", &m.menuTypedSearch}};
        this->fillChoices();
        this->exitMenu_ = &m.menuMain;
    }

    virtual void handleEnter() {
        std::string choice = this->choice();
        if (choice == "Forward")
            this->data_.log().next(this->data_.searchFilter());
        if (choice == "Backward")
            this->data_.log().prev(this->data_.searchFilter());
        Base::handleEnter();
    }
};

template <class AllMenus, class Graph, class VisualLog>
struct MenuEditFilter : MenuBase<AllMenus, Graph, VisualLog> {
    using Base = MenuBase<AllMenus, Graph, VisualLog>;
    using Data = typename Base::Data;

    MenuEditFilter(AllMenus &m, VisualizerData<Graph, VisualLog> &data)
        : MenuBase<AllMenus, Graph, VisualLog>(m, data) {
        this->enterMap_ = {{"All", &m.menuEditFilter},
                           {"None", &m.menuEditFilter}};
        for (auto &el : VisualLog::AlgorithmLog::AlgorithmEvent::eventTypeStr)
            this->enterMap_.push_back({el, &m.menuFilter});
        this->fillChoices();
        this->nonSelectable_ = {"All", "None"};
        this->exitMenu_ = &m.menuFilter;
    }

    virtual bool multi() const override { return true; }

    virtual void handleEnter() {
        std::string choice = this->choice(); (void)choice;
        if (choice == "All")
            this->selectAll();
        else if (choice == "None")
            this->selectNone();
        else {
            this->data_.filter().filterEventType().set(
                menuChoices(this->m_.raw()));
            this->data_.log().setFilter(this->data_.filter());
            this->data_.typist().fillEventsPad();
        }
        Base::handleEnter();
    }
};

template<class Graph, class VisualLog>
struct AllMenus {
    AllMenus(VisualizerData<Graph, VisualLog> &data)
        : menuMain(*this, data), menuRun(*this, data), menuSearch(*this, data),
          menuFilter(*this, data), menuGo(*this, data), menuSpeed(*this, data),
          menuTypedSearch(*this, data), menuEditFilter(*this, data) {
        setMenu(&menuMain, data.typist());
    }
    ~AllMenus() { destroyMenu(raw_); }
    MenuMain<AllMenus, Graph, VisualLog> menuMain;
    MenuRun<AllMenus, Graph, VisualLog> menuRun;
    MenuSearch<AllMenus, Graph, VisualLog> menuSearch;
    MenuFilter<AllMenus, Graph, VisualLog> menuFilter;
    MenuGo<AllMenus, Graph, VisualLog> menuGo;
    MenuSpeed<AllMenus, Graph, VisualLog> menuSpeed;
    MenuTypedSearch<AllMenus, Graph, VisualLog> menuTypedSearch;
    MenuEditFilter<AllMenus, Graph, VisualLog> menuEditFilter;
    bool hideFiltered = true;

    void handleEnter() { m_->handleEnter(); }
    void handleEsc() { m_->handleEsc(); }

    void setMenu(MenuBase<AllMenus, Graph, VisualLog> *newMenu,
                 Typist<VisualLog> &typist) {
        if (newMenu == m_) return;
        destroyMenu(raw_);
        menuItems_.clear();
        raw_ = createMenu(typist.commandsPad(), menuItems_, newMenu->choices(),
                          newMenu->nonSelectable(),
                          newMenu->data().filter().filterEventType().get(),
                          maxMenuRows_, newMenu->multi());
        m_ = newMenu;
        typist.setMenu(raw_);
    }

    MENU *raw() { return raw_; }

    std::vector<ITEM *> menuItems() {return menuItems_;}

private:
    MENU *raw_ = nullptr;
    MenuBase<AllMenus, Graph, VisualLog> *m_;
    std::vector<ITEM *> menuItems_;
    int maxMenuRows_ = 3;
};

#endif
