#include <iostream>
#include <memory>
#include <vector>
#include <set>
#include <unordered_set>
//#include <pair>
#include <algorithm>
#include <sstream>
#include <iomanip>

int nPancakes() { return 4; }

struct Pancake;

template <typename State_ = Pancake> struct Cost {
    using State = State_; ///< The state type, represents the domain.
    using CostType = typename State::CostType; ///< Action cost type.

    /// Constructor.
    Cost(CostType) {}

    /// Returns cost of the action that leads to the neighbor state.
    /// \return Cost of the action that leads to the neighbor state.
    constexpr CostType cost() const { return CostType{1}; }
};

template <typename State_ = Pancake>
struct ActionNeighbor: Cost<State_>  {
    using State = State_; ///< The state type, represents the domain.
    using MyCost = Cost<State_>;
    using Action = typename State::Action; ///< Action type.
    using CostType = typename State::CostType; ///< Action cost type.

    /// Initializes the neighbor based on the neighbor state and cost
    /// of the action that leads to that state.
    /// \param a The action, which must be a right value.
    /// \param c Cost of the action that leads to \c s.
    ActionNeighbor(Action &&a, CostType c = CostType{1}) : MyCost(c), a_{a} {}
    /// Returns the action.
    /// \return Const reference to the action.
    const Action &action() { return a_; }

private:
    Action a_; ///< The action.
};

template <class State = Pancake>
struct ActionsGenerator {
    using Neighbor = typename State::ANeighbor;

    std::vector<const Neighbor> successors(const State &s) const {
        return s.actionSuccessors();
    }
};

struct Pancake {
    using CostType = int;
    using ANeighbor = ActionNeighbor<const Pancake>; ///< Action neighbor type.
    using Action = int;

    Pancake() : pancakes_(nPancakes()) {
        int i = -1;
        for (auto &el: pancakes_) el = ++i;
    }

    Pancake (const Pancake &) = default;
    Pancake &operator=(const Pancake &) = default;
    const std::vector<int> &getPancakes() const { return pancakes_; }

    Pancake &apply(Action a) {
        std::reverse(pancakes_.begin(), pancakes_.begin() + a + 1);
        return *this;
    }

    std::vector<Action> actions() const {
        std::vector<Action> res;
        for (auto i = 1U; i != pancakes_.size(); ++i)
            res.push_back(i);
        return res;
    }

    std::vector<ANeighbor> actionSuccessors() const {
        std::vector<ANeighbor> res;
        for (auto a : actions()) res.push_back(std::move(a));
        return res;
    }

private:
    std::vector<int> pancakes_; ///< The underlying state representation.
};

int main() {
    ActionsGenerator<> g{}; (void)g;
    Pancake s{};
    auto neighbors = g.successors(s);      // does not compile
    //auto neighbors = s.actionSuccessors(); // compiles fine
    std::cout << neighbors[1].action() << std::endl;
    return 0;
}
