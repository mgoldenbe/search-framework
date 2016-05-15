#ifndef MENUS_H
#define MENUS_H

#include "visualizer_data.h"

template <class AllMenus, class Node>
struct MenuBase {
    using Base = MenuBase<AllMenus, Node>;
    using Data = VisualizerData<Node>;

    MenuBase(AllMenus &m, Data &data)
        : m_(m), data_(data) {}

    const std::vector<std::string> &choices() const { return choices_; }

    const std::vector<std::string> &nonSelectable() const {
        return nonSelectable_;
    }

    virtual bool multi() const { return false; }

    Data &data() { return data_; }

    Form &form() { return form_; }

    bool formEmpty() const {return form_.empty();}

    virtual void handleEnter() {
        std::string choice = this->choice();
        Base *m = std::find_if(enterMap_.begin(), enterMap_.end(),
                               [choice](std::pair<std::string, Base *> &p) {
            return p.first == choice;
        })->second;
        m_.setMenu(m, data_.logWindow());
    }

    void handleEsc() { m_.setMenu(exitMenu_, data_.logWindow()); }

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
    Form form_;

    void fillChoices() {
        choices_.clear();
        std::transform(
            enterMap_.begin(), enterMap_.end(), std::back_inserter(choices_),
            [](const std::pair<std::string, Base *> &p) { return p.first; });
    }

    std::string choice() const { return menuChoice(m_.raw()); }
};

template <class AllMenus, class Node>
struct MenuMain
    : public MenuBase<AllMenus, Node> {
    using Base = MenuBase<AllMenus, Node>;
    using Data = typename Base::Data;

    MenuMain(AllMenus &m, VisualizerData<Node> &data)
        : MenuBase<AllMenus, Node>(m, data) {
        this->enterMap_ = {{"Run", &m.menuRun},
                           {"Search", &m.menuEnterState},
                           {"Filter", &m.menuFilter},
                           {"Layout", &m.menuMain}};
        this->fillChoices();
        this->exitMenu_ = &m.menuMain;
    }

    virtual void handleEnter() {
        std::string choice = this->choice();
        if (choice == "Layout")
            this->data_.drawer().makeLayout();
        Base::handleEnter();
    }
};

template <class AllMenus, class Node>
struct MenuRun : MenuBase<AllMenus, Node> {
    using Base = MenuBase<AllMenus, Node>;
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
        if (choice == "Step Forward")
            this->data_.log().next(this->data_.drawer());
        if (choice == "Step Backward")
            this->data_.log().prev(this->data_.drawer());
        if (choice == "Reset")
            this->data_.log().move(0, this->data_.drawer(), true);
        Base::handleEnter();
    }
};

template <class AllMenus, class Node>
struct MenuSearch : MenuBase<AllMenus, Node> {
    MenuSearch(AllMenus &m,
               VisualizerData<Node> &data)
        : MenuBase<AllMenus, Node>(m, data) {
        this->enterMap_ = {{"By event", &m.menuTypedSearch},
                           {"By state", &m.menuTypedSearch},
                           {"By data", &m.menuTypedSearch}};
        this->fillChoices();
        this->exitMenu_ = &m.menuMain;
    }
};

template <class AllMenus, class Node>
struct MenuEnterState : MenuBase<AllMenus, Node> {
    using Base = MenuBase<AllMenus, Node>;
    using Data = typename Base::Data;
    using MyVisualLog = VisualLog<Node>;

    MenuEnterState(AllMenus &m,
                   VisualizerData<Node> &data)
        : MenuBase<AllMenus, Node>(m, data) {
        EditField editField{this->data_.drawer().graphics().display,
                            this->data_.logWindow().menuPad(),
                            0,
                            0,
                            1,
                            40,
                            "Enter state: "};
        this->enterMap_ = {{" ", &m.menuTypedSearch}};
        this->fillChoices();
        this->exitMenu_ = &m.menuMain;

        this->form_.addField(editField);
    }

    virtual void handleEnter() {
        auto &stateStr = this->form_.get(0);
        auto &filter = this->data_.searchFilter().filterState();
        if (stateStr.empty()) {
            this->data_.logWindow().message("Cleared search filter");
            filter.reset();
        } else {
            auto state = std::make_shared<typename MyVisualLog::State>(stateStr);
            filter.set(state);
            this->data_.logWindow().message("Set search filter: " + str(*state));
        }
        Base::handleEnter(); // Need to do this before the menu entry changed
    }

private:
    std::string label_ = "Enter state: ";
};

template <class AllMenus, class Node>
struct MenuFilter : MenuBase<AllMenus, Node> {
    using Base = MenuBase<AllMenus, Node>;
    using Data = typename Base::Data;

    MenuFilter(AllMenus &m,
               VisualizerData<Node> &data)
        : MenuBase<AllMenus, Node>(m, data) {
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

            this->data_.logWindow().hideFiltered(this->m_.hideFiltered);
            this->data_.logWindow().fillEventsPad();

            // Cause the item to update
            menu_driver(this->m_.raw(), REQ_LEFT_ITEM);
            menu_driver(this->m_.raw(), REQ_RIGHT_ITEM);
        }

        Base::handleEnter(); // Need to do this before the menu entry changed
    }
};

template <class AllMenus, class Node>
struct MenuGo : MenuBase<AllMenus, Node> {
    using Base = MenuBase<AllMenus, Node>;
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

template <class AllMenus, class Node>
struct MenuSpeed : MenuBase<AllMenus, Node> {
    using Base = MenuBase<AllMenus, Node>;
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

template <class AllMenus, class Node>
struct MenuTypedSearch : MenuBase<AllMenus, Node> {
    using Base = MenuBase<AllMenus, Node>;
    using Data = typename Base::Data;

    MenuTypedSearch(AllMenus &m,
                    VisualizerData<Node> &data)
        : MenuBase<AllMenus, Node>(m, data) {
        this->enterMap_ = {{"Forward", &m.menuTypedSearch},
                           {"Backward", &m.menuTypedSearch}};
        this->fillChoices();
        this->exitMenu_ = &m.menuMain;
    }

    virtual void handleEnter() {
        std::string choice = this->choice();
        if (choice == "Forward")
            this->data_.log().next(this->data_.searchFilter(),
                                   this->data_.drawer());
        if (choice == "Backward")
            this->data_.log().prev(this->data_.searchFilter(),
                                   this->data_.drawer());
        Base::handleEnter();
    }
};

template <class AllMenus, class Node>
struct MenuEditFilter : MenuBase<AllMenus, Node> {
    using Base = MenuBase<AllMenus, Node>;
    using Data = typename Base::Data;

    MenuEditFilter(AllMenus &m,
                   VisualizerData<Node> &data)
        : MenuBase<AllMenus, Node>(m, data) {
        this->enterMap_ = {{"All", &m.menuEditFilter},
                           {"None", &m.menuEditFilter}};
        for (auto &el : data.log().algorithmLog().eventStrings())
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
            this->data_.filter().filterEventStr().set(
                menuChoices(this->m_.raw()));
            this->data_.log().setFilter(this->data_.filter(),
                                        this->data_.drawer());
            this->data_.logWindow().fillEventsPad();
        }
        Base::handleEnter();
    }
};

template<class Node>
struct AllMenus {
    AllMenus(VisualizerData<Node> &data)
        : menuMain(*this, data), menuRun(*this, data), menuSearch(*this, data),
          menuEnterState(*this, data), menuFilter(*this, data),
          menuGo(*this, data), menuSpeed(*this, data),
          menuTypedSearch(*this, data), menuEditFilter(*this, data) {
        setMenu(&menuMain, data.logWindow());
    }
    ~AllMenus() { destroyMenu(raw_); }

    bool hideFiltered = true;
    MenuMain<AllMenus, Node> menuMain;
    MenuRun<AllMenus, Node> menuRun;
    MenuSearch<AllMenus, Node> menuSearch;
    MenuEnterState<AllMenus, Node> menuEnterState;
    MenuFilter<AllMenus, Node> menuFilter;
    MenuGo<AllMenus, Node> menuGo;
    MenuSpeed<AllMenus, Node> menuSpeed;
    MenuTypedSearch<AllMenus, Node> menuTypedSearch;
    MenuEditFilter<AllMenus, Node> menuEditFilter;

    void handleEnter() { m_->handleEnter(); }
    void handleEsc() { m_->handleEsc(); }

    void setMenu(MenuBase<AllMenus, Node> *newMenu,
                 LogWindow<Node> &logWindow) {
        if (newMenu == m_) return;

        destroyMenu(raw_);
        menuItems_.clear();
        raw_ = createMenu(logWindow.menuPad(), menuItems_, newMenu->choices(),
                          newMenu->nonSelectable(),
                          newMenu->data().filter().filterEventStr().get(),
                          maxMenuRows_, newMenu->multi());
        m_ = newMenu;
        logWindow.setMenu(raw_, &newMenu->form());
    }

    MenuBase<AllMenus, Node> *curMenu() {return m_;}

    Form &curForm() {return m_->form();}

    MENU *raw() { return raw_; }

    std::vector<ITEM *> menuItems() {return menuItems_;}

private:
    MENU *raw_ = nullptr;
    MenuBase<AllMenus, Node> *m_ = nullptr;
    std::vector<ITEM *> menuItems_;
    int maxMenuRows_ = 3;
};

#endif
