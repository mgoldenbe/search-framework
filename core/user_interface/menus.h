#ifndef MENUS_H
#define MENUS_H

/// \file
/// \brief Implementation of the menus appearing at the bottom of the log window.
/// \author Meir Goldenberg

#include "visualizer_data.h"

namespace slb {
namespace core {
namespace ui {

/// The base class for all the menus.
/// \tparam AllMenus The type for holding all the menus.
/// \tparam Node The search node type.
template <class AllMenus, class Node>
struct MenuBase {
    /// Just a short name for the base class for all the menus.
    using Base = MenuBase<AllMenus, Node>;

    /// The base of \ref ui::Visualizer holding the visualizer's constituent
    /// components.
    using Data = VisualizerData<Node>;

    /// Initializes with a back-reference to the whole menu and reference to the
    /// constituent components of the visualizer.
    /// \param m Back-reference to the whole menu.
    /// \param data reference to the constituent components of the visualizer.
    MenuBase(AllMenus &m, Data &data)
        : m_(m), data_(data) {}

    /// Returns the strings representing the choices for the menu.
    /// \return Vector of strings representing the choices for the menu.
    const std::vector<std::string> &choices() const { return choices_; }

    /// Returns the strings representing the disabled choices for the menu.
    /// \return Vector of strings representing the disabled choices for the menu.
    const std::vector<std::string> &nonSelectable() const {
        return nonSelectable_;
    }

    /// Returns whether the menu allows the selection of several choices.
    /// \return \c true if the menu allows the selection of several choices.
    virtual bool multi() const { return false; }

    /// Returns reference to the constituent components of the visualizer.
    /// \return Reference to the constituent components of the visualizer.
    Data &data() { return data_; }

    /// Returns reference to the form of the menu.
    /// \return Reference to the form of the menu.
    Form &form() { return form_; }

    /// Returns whether the currently active form of the menu is empty.
    /// \return \c true if the currently active form of the menu is empty does
    /// not have any fields.
    bool formEmpty() const {return form_.empty();}

    /// Handles the pressed Enter key.
    virtual void handleEnter() {
        std::string choice = this->choice();
        Base *m = std::find_if(enterMap_.begin(), enterMap_.end(),
                               [choice](std::pair<std::string, Base *> &p) {
            return p.first == choice;
        })->second;
        m_.setMenu(m, data_.logWindow());
    }

    /// Handles the pressed Esc key.
    void handleEsc() { m_.setMenu(exitMenu_, data_.logWindow()); }

    /// Marks all choices as selected.
    void selectAll() {
        for (auto item: m_.menuItems()) set_item_value(item, true);
    }

    /// Marks all choices as not selected.
    void selectNone() {
        for (auto item: m_.menuItems()) set_item_value(item, false);
    }

protected:
    /// Back-reference to the whole menu.
    AllMenus &m_;

    /// Reference to the constituent components of the visualizer.
    Data &data_;

    /// For each choice, the menu that needs to be entered.
    std::vector<std::pair<std::string, Base *>> enterMap_;

    /// All the choices.
    std::vector<std::string> choices_;

    /// The choices that cannot be selected.
    std::vector<std::string> nonSelectable_;

    /// The parent of the menu, which will be switched to when the menu is
    /// exited by means of pressing Esc.
    Base *exitMenu_;

    /// The form of the menu.
    Form form_;

    /// Fills the \ref choices_ based on \ref enterMap_.
    void fillChoices() {
        choices_.clear();
        std::transform(
            enterMap_.begin(), enterMap_.end(), std::back_inserter(choices_),
            [](const std::pair<std::string, Base *> &p) { return p.first; });
    }

    /// Returns the currently active choice.
    /// \return the currently active choice.
    std::string choice() const {
        auto res = menuChoice(m_.raw());
        assert(res.size());
        return res;
    }
};

/// The root menu.
/// \tparam AllMenus The type for holding all the menus.
/// \tparam Node The search node type.
template <class AllMenus, class Node>
struct MenuMain : public MenuBase<AllMenus, Node> {
    /// Just a short name for the base class for all the menus.
    using Base = MenuBase<AllMenus, Node>;

    /// The base of \ref ui::Visualizer holding the visualizer's constituent
    /// components.
    using Data = typename Base::Data;

    /// Initializes the menu.
    /// \param m Back-reference to the whole menu.
    /// \param data reference to the constituent components of the visualizer.
    MenuMain(AllMenus &m, VisualizerData<Node> &data)
        : MenuBase<AllMenus, Node>(m, data) {
        this->enterMap_ = {{"Run", &m.menuRun},
                           {"Search", &m.menuEnterState},
                           {"Filter", &m.menuFilter},
                           {"Layout", &m.menuLayout}};
        this->fillChoices();
        this->exitMenu_ = &m.menuMain;
    }

    /// Handles the pressed Enter key.
    virtual void handleEnter() { Base::handleEnter(); }
};

/// The menu reached when choosing the Run option from the root menu.
/// \tparam AllMenus The type for holding all the menus.
/// \tparam Node The search node type.
template <class AllMenus, class Node>
struct MenuRun : MenuBase<AllMenus, Node> {
    /// Just a short name for the base class for all the menus.
    using Base = MenuBase<AllMenus, Node>;

    /// The base of \ref ui::Visualizer holding the visualizer's constituent
    /// components.
    using Data = typename Base::Data;

    /// Initializes the menu.
    /// \param m Back-reference to the whole menu.
    /// \param data reference to the constituent components of the visualizer.
    MenuRun(AllMenus &m, Data &data) : Base(m, data) {
        this->enterMap_ = {{"Go", &m.menuGo},
                           {"Speed (steps/sec.)", &m.menuSpeed},
                           {"Step", &m.menuStep},
                           {"Jump", &m.menuJump}};
        this->fillChoices();
        this->exitMenu_ = &m.menuMain;
    }

    /// Handles the pressed Enter key.
    virtual void handleEnter() {
        std::string choice = this->choice();
        if (choice == "Go") this->data_.state(Data::VISUALIZER_STATE::GO);
        Base::handleEnter();
    }
};

/// The menu reached when choosing Run->Step.
/// \tparam AllMenus The type for holding all the menus.
/// \tparam Node The search node type.
template <class AllMenus, class Node>
struct MenuStep : MenuBase<AllMenus, Node> {
    /// Just a short name for the base class for all the menus.
    using Base = MenuBase<AllMenus, Node>;

    /// The base of \ref ui::Visualizer holding the visualizer's constituent
    /// components.
    using Data = typename Base::Data;

    /// Initializes the menu.
    /// \param m Back-reference to the whole menu.
    /// \param data reference to the constituent components of the visualizer.
    MenuStep(AllMenus &m, Data &data) : Base(m, data) {
        this->enterMap_ = {{"Step Forward", &m.menuStep},
                           {"Step Backward", &m.menuStep}};
        this->fillChoices();
        this->exitMenu_ = &m.menuRun;
    }

    /// Handles the pressed Enter key.
    virtual void handleEnter() {
        std::string choice = this->choice();
        if (choice == "Step Forward")
            this->data_.log().next(this->data_.drawer());
        if (choice == "Step Backward")
            this->data_.log().prev(this->data_.drawer());
        Base::handleEnter();
    }
};

/// The menu reached when choosing Run->Jump.
/// \tparam AllMenus The type for holding all the menus.
/// \tparam Node The search node type.
template <class AllMenus, class Node>
struct MenuJump : MenuBase<AllMenus, Node> {
    /// Just a short name for the base class for all the menus.
    using Base = MenuBase<AllMenus, Node>;

    /// The base of \ref ui::Visualizer holding the visualizer's constituent
    /// components.
    using Data = typename Base::Data;

    /// Initializes the menu.
    /// \param m Back-reference to the whole menu.
    /// \param data reference to the constituent components of the visualizer.
    MenuJump(AllMenus &m, Data &data) : Base(m, data) {
        this->enterMap_ = {{"Step", &m.menuJumpStep},
                           {"Begin", &m.menuRun},
                           {"End", &m.menuRun}};
        this->fillChoices();
        this->exitMenu_ = &m.menuRun;
    }

    /// Handles the pressed Enter key.
    virtual void handleEnter() {
        std::string choice = this->choice();
        if (choice == "Begin")
            this->data_.log().move(0, this->data_.drawer(), true);
        if (choice == "End")
            this->data_.log().move(this->data_.log().nEvents(),
                                   this->data_.drawer(), true);
        Base::handleEnter();
    }
};

/// The menu reached when choosing Run->Jump->Step
/// \tparam AllMenus The type for holding all the menus.
/// \tparam Node The search node type.
template <class AllMenus, class Node>
struct MenuJumpStep : MenuBase<AllMenus, Node> {
    /// Just a short name for the base class for all the menus.
    using Base = MenuBase<AllMenus, Node>;

    /// The base of \ref ui::Visualizer holding the visualizer's constituent
    /// components.
    using Data = typename Base::Data;

    /// The type of the log of visual events.
    using MyVisualLog = VisualLog<Node>;

    /// Initializes the menu.
    /// \param m Back-reference to the whole menu.
    /// \param data reference to the constituent components of the visualizer.
    MenuJumpStep(AllMenus &m, VisualizerData<Node> &data)
        : MenuBase<AllMenus, Node>(m, data) {
        EditField editField{this->data_.drawer().graphics().display,
                            this->data_.logWindow().menuPad(),
                            0,
                            0,
                            1,
                            40,
                            "Enter step: "};
        this->enterMap_ = {{" ", &m.menuRun}};
        this->fillChoices();
        this->exitMenu_ = &m.menuRun;

        this->form_.addField(editField);
    }

    /// Handles the pressed Enter key.
    virtual void handleEnter() {
        auto &stepStr = this->form_.get(0);
        int step;
        try {
            step = stoi(stepStr);
            if (step < 0 || step > this->data_.log().nEvents() - 1)
                throw std::invalid_argument("");
            this->data_.log().move(step, this->data_.drawer(), true);
        }
        catch (...) {
            this->data_.logWindow().message("Bad step number");
        }
        Base::handleEnter(); // Need to do this before the menu entry changed
    }
};

/// The menu reached when choosing the Search option from the root menu.
/// \tparam AllMenus The type for holding all the menus.
/// \tparam Node The search node type.
template <class AllMenus, class Node>
struct MenuEnterState : MenuBase<AllMenus, Node> {
    /// Just a short name for the base class for all the menus.
    using Base = MenuBase<AllMenus, Node>;

    /// The base of \ref ui::Visualizer holding the visualizer's constituent
    /// components.
    using Data = typename Base::Data;

    /// The type of the log of visual events.
    using MyVisualLog = VisualLog<Node>;

    /// Initializes the menu.
    /// \param m Back-reference to the whole menu.
    /// \param data reference to the constituent components of the visualizer.
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
        this->enterMap_ = {{" ", &m.menuSearchDirection}};
        this->fillChoices();
        this->exitMenu_ = &m.menuMain;

        this->form_.addField(editField);
    }

    /// Handles the pressed Enter key.
    virtual void handleEnter() {
        auto &stateStr = this->form_.get(0);
        auto &filter = this->data_.searchFilter().filterState();
        if (stateStr.empty()) {
            this->data_.logWindow().message("Cleared search filter");
            filter.reset();
        } else {
            auto state =
                make_deref_shared<const typename MyVisualLog::State>(stateStr);
            filter.set(state);
            this->data_.logWindow().message("Set search filter: " + str(*state));
        }
        Base::handleEnter(); // Need to do this before the menu entry changed
    }
};

/// The menu reached when choosing the Filter option from the root menu.
/// \tparam AllMenus The type for holding all the menus.
/// \tparam Node The search node type.
template <class AllMenus, class Node>
struct MenuFilter : MenuBase<AllMenus, Node> {
    /// Just a short name for the base class for all the menus.
    using Base = MenuBase<AllMenus, Node>;

    /// The base of \ref ui::Visualizer holding the visualizer's constituent
    /// components.
    using Data = typename Base::Data;

    /// Initializes the menu.
    /// \param m Back-reference to the whole menu.
    /// \param data reference to the constituent components of the visualizer.
    MenuFilter(AllMenus &m,
               VisualizerData<Node> &data)
        : MenuBase<AllMenus, Node>(m, data) {
        this->enterMap_ = {{"Edit", &m.menuEditFilter},
                           {m.hideFiltered ? "Show" : "Hide", &m.menuFilter}};
        this->fillChoices();
        this->exitMenu_ = &m.menuMain;
    }

    /// Handles the pressed Enter key.
    virtual void handleEnter() {
        std::string choice = this->choice();
        if (choice == "Show" || choice == "Hide") {
            this->m_.hideFiltered = !this->m_.hideFiltered;
            this->enterMap_[1].first = this->m_.hideFiltered ? "Show" : "Hide";
            const std::string &updatedItem = this->enterMap_[1].first;
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

/// The menu reached when choosing Run->Go from the root menu.
/// \tparam AllMenus The type for holding all the menus.
/// \tparam Node The search node type.
template <class AllMenus, class Node>
struct MenuGo : MenuBase<AllMenus, Node> {
    /// Just a short name for the base class for all the menus.
    using Base = MenuBase<AllMenus, Node>;

    /// The base of \ref ui::Visualizer holding the visualizer's constituent
    /// components.
    using Data = typename Base::Data;

    /// Initializes the menu.
    /// \param m Back-reference to the whole menu.
    /// \param data reference to the constituent components of the visualizer.
    MenuGo(AllMenus &m, Data &data) : Base(m, data) {
        this->enterMap_ = {{"Pause", &m.menuRun}};
        this->fillChoices();
        this->exitMenu_ = &m.menuMain;
    }

    /// Handles the pressed Enter key.
    virtual void handleEnter() {
        this->data_.state(Data::VISUALIZER_STATE::PAUSE);
        Base::handleEnter();
    }
};

/// The menu reached when choosing Run->Speed from the root menu.
/// \tparam AllMenus The type for holding all the menus.
/// \tparam Node The search node type.
template <class AllMenus, class Node>
struct MenuSpeed : MenuBase<AllMenus, Node> {
    /// Just a short name for the base class for all the menus.
    using Base = MenuBase<AllMenus, Node>;

    /// The base of \ref ui::Visualizer holding the visualizer's constituent
    /// components.
    using Data = typename Base::Data;

    /// Initializes the menu.
    /// \param m Back-reference to the whole menu.
    /// \param data reference to the constituent components of the visualizer.
    MenuSpeed(AllMenus &m, Data &data) : Base(m, data) {
        for (auto el : std::vector<std::string>{"1", "2", "5", "10", "50",
                                                "200", "500", "1000"})
            this->enterMap_.push_back({el, &m.menuRun});

        this->fillChoices();
        this->exitMenu_ = &m.menuRun;
    }

    /// Handles the pressed Enter key
    virtual void handleEnter() {
        int speed = boost::lexical_cast<int>(this->choice());
        this->data_.speed(speed);
        Base::handleEnter();
    }
};

/// The menu for choosing the search direction after specifying the search in
/// the search menu (reached when choosing the Search option from the root menu).
/// \tparam AllMenus The type for holding all the menus.
/// \tparam Node The search node type.
template <class AllMenus, class Node>
struct MenuSearchDirection : MenuBase<AllMenus, Node> {
    /// Just a short name for the base class for all the menus.
    using Base = MenuBase<AllMenus, Node>;

    /// The base of \ref ui::Visualizer holding the visualizer's constituent
    /// components.
    using Data = typename Base::Data;

    /// Initializes the menu.
    /// \param m Back-reference to the whole menu.
    /// \param data reference to the constituent components of the visualizer.
    MenuSearchDirection(AllMenus &m,
                    VisualizerData<Node> &data)
        : MenuBase<AllMenus, Node>(m, data) {
        this->enterMap_ = {{"Forward", &m.menuSearchDirection},
                           {"Backward", &m.menuSearchDirection}};
        this->fillChoices();
        this->exitMenu_ = &m.menuMain;
    }

    /// Handles the pressed Enter key
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

/// The menu reached when choosing Filter->Edit from the root menu.
/// \tparam AllMenus The type for holding all the menus.
/// \tparam Node The search node type.
template <class AllMenus, class Node>
struct MenuEditFilter : MenuBase<AllMenus, Node> {
    /// Just a short name for the base class for all the menus.
    using Base = MenuBase<AllMenus, Node>;

    /// The base of \ref ui::Visualizer holding the visualizer's constituent
    /// components.
    using Data = typename Base::Data;

    /// Initializes the menu.
    /// \param m Back-reference to the whole menu.
    /// \param data reference to the constituent components of the visualizer.
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

    /// Returns \c true because this menu is a multi-choice one.
    /// \return \c true because this menu is a multi-choice one.
    virtual bool multi() const override { return true; }

    /// Handles the pressed Enter key.
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

/// /// The menu reached when choosing Layout from the root menu.
/// \tparam AllMenus The type for holding all the menus.
/// \tparam Node The search node type.
template <class AllMenus, class Node>
struct MenuLayout : public MenuBase<AllMenus, Node> {
    /// Just a short name for the base class for all the menus.
    using Base = MenuBase<AllMenus, Node>;

    /// The base of \ref ui::Visualizer holding the visualizer's constituent
    /// components.
    using Data = typename Base::Data;

    /// Initializes the menu.
    /// \param m Back-reference to the whole menu.
    /// \param data reference to the constituent components of the visualizer.
    MenuLayout(AllMenus &m, VisualizerData<Node> &data)
        : MenuBase<AllMenus, Node>(m, data) {
        this->enterMap_ = {{"Recompute", &m.menuLayout},
                           {"Toggle labels", &m.menuMain}};
        this->fillChoices();
        this->exitMenu_ = &m.menuMain;
    }

    /// Handles the pressed Enter key.
    virtual void handleEnter() {
        std::string choice = this->choice();
        if (choice == "Recompute")
            this->data_.drawer().makeLayout(true);
        if (choice == "Toggle labels") {
            this->m_.showLabels = !this->m_.showLabels;
            this->data_.drawer().showLabels(this->m_.showLabels);
        }
        Base::handleEnter();
    }
};

/// A holder for all the menus with an indicator of which menu and form are
/// currently active.
/// \tparam Node The search node type.
template<class Node>
struct AllMenus {
    /// Initializes all the menus and sets the root menu as the current menu.
    /// \param data reference to the constituent components of the visualizer.
    AllMenus(VisualizerData<Node> &data)
        : menuMain(*this, data), menuRun(*this, data),
          menuEnterState(*this, data), menuFilter(*this, data),
          menuGo(*this, data), menuSpeed(*this, data),
          menuSearchDirection(*this, data), menuEditFilter(*this, data),
          menuStep(*this, data), menuJump(*this, data),
          menuJumpStep(*this, data), menuLayout(*this, data) {
        setMenu(&menuMain, data.logWindow());
    }

    /// Destroys the current menu.
    ~AllMenus() { destroyMenu(raw_); }

    /// Indicates wither the filtered out events should be shown in the events
    /// pad of the log window. Always kept in sync with
    /// \ref LogWindow::hideFiltered_.
    bool hideFiltered = true;

    /// Indicates wither the vertex and edge labels should be shown.
    /// Always kept in sync with \ref Drawer::showLabels_.
    bool showLabels = false;

    /// The root menu.
    MenuMain<AllMenus, Node> menuMain;

    /// The menu reached when choosing the Run option from the root menu.
    MenuRun<AllMenus, Node> menuRun;

    /// The menu reached when choosing the Search option from the root menu.
    MenuEnterState<AllMenus, Node> menuEnterState;

    /// The menu reached when choosing the Filter option from the root menu.
    MenuFilter<AllMenus, Node> menuFilter;

    /// The menu reached when choosing Run->Go from the root menu.
    MenuGo<AllMenus, Node> menuGo;

    /// The menu reached when choosing Run->Speed from the root menu.
    MenuSpeed<AllMenus, Node> menuSpeed;

    /// The menu for choosing the search direction after specifying the search in
    /// the search menu (reached when choosing the Search option from the root
    /// menu).
    MenuSearchDirection<AllMenus, Node> menuSearchDirection;

    /// The menu reached when choosing Filter->Edit from the root menu.
    MenuEditFilter<AllMenus, Node> menuEditFilter;

    /// The menu reached when choosing Run->Step from the root menu.
    MenuStep<AllMenus, Node> menuStep;

    /// The menu reached when choosing Run->Jump from the root menu.
    MenuJump<AllMenus, Node> menuJump;

    /// The menu reached when choosing Run->Jump->Step from the root menu.
    MenuJumpStep<AllMenus, Node> menuJumpStep;

    /// The menu reached when choosing Run->Layout from the root menu.
    MenuLayout<AllMenus, Node> menuLayout;

    /// Handles the pressed Enter key by passing it to the handler of the
    /// currently active menu.
    void handleEnter() { m_->handleEnter(); }

    /// Handles the pressed Esc key by passing it to the handler of the
    /// currently active menu.
    void handleEsc() { m_->handleEsc(); }

    /// Sets the new acive menu. The previously active menu is destroyed and the
    /// new active menu is created. This was the easiest way to reset things.
    /// \param newMenu The newly active menu.
    /// \param logWindow Reference to the log window.
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

    /// Returns the currently active menu.
    /// \return Pointer to the currently active menu.
    MenuBase<AllMenus, Node> *curMenu() {return m_;}

    /// Returns the currently active form.
    /// \return Reference to the currently active menu.
    Form &curForm() {return m_->form();}

    /// Returns the currently active menu in the raw \c ncurses format.
    /// \return Pointer to the currently active menu in the raw \c ncurses
    /// format.
    MENU *raw() { return raw_; }

    /// Returns the items of the active menu in the raw \c ncurses format.
    /// \return Vector of pointers to items (in the raw \c ncurses format.) of
    /// the currently active menu.
    std::vector<ITEM *> menuItems() {return menuItems_;}

private:
    /// The currently active menu in the raw \c ncurses format.
    MENU *raw_ = nullptr;

    /// The currently active menu.
    MenuBase<AllMenus, Node> *m_ = nullptr;

    /// Vector of pointers to items (in the raw \c ncurses format.) of the
    /// currently active menu.
    std::vector<ITEM *> menuItems_;

    /// Max number of rows in the menu.
    int maxMenuRows_ = 3;
};

} // namespace
} // namespace
} // namespace

#endif
