#ifndef EXPLICIT_STATE_H
#define EXPLICIT_STATE_H

// The base class for a state in an explicit domain
template<class ExplicitSpace>
struct ExplicitState {
    using CostType = typename ExplicitSpace::CostType;
    using MyType = ExplicitState<ExplicitSpace>;
    using StateType = typename ExplicitSpace::StateType;

    ///@name Construction and Assignment
    //@{
    ExplicitState() : state_(space()->defaultState()) {}
    ExplicitState(const StateType &state) : state_(state) {}
    ExplicitState(const std::string &s) : state_(space()->state(s)) {}
    ExplicitState(const MyType &) = default;
    ExplicitState &operator=(const MyType &rhs) = default;
    //@}

    ///@name Read-Only Services
    //@{
    template<class Neighbor>
    std::vector<Neighbor> successors() const {
        std::vector<Neighbor> res;
        for (auto &n: space_->template neighbors<Neighbor>(state_))
            res.push_back(std::move(n));
        return res;
    }

    std::size_t hash() const {
        boost::hash<StateType> myHash;
        return myHash(state_);
    }

    const StateType &raw() const { return state_; }
    //@}

    template <typename charT>
    friend std::basic_ostream<charT> &operator<<(std::basic_ostream<charT> &o,
                                                 const MyType &rhs) {
        return o << rhs.space()->stateStr(rhs.state_);
    }

    friend bool operator==(const MyType &lhs, const MyType &rhs) {
        return lhs.state_ == rhs.state_;
    }

    static void initSpace(const std::string fileName) {
        space_ = std::unique_ptr<ExplicitSpace>(new ExplicitSpace(fileName));
    }

    static StateType random() { return space()->random(); }

    static const std::unique_ptr<ExplicitSpace> &space() {
        if (space_ == nullptr)
            throw std::logic_error("Explicit space has not been initialized");
        return space_;
    }
private:
    StateType state_;
    static std::unique_ptr<ExplicitSpace> space_;
};

template<class ExplicitSpace>
std::unique_ptr<ExplicitSpace> ExplicitState<ExplicitSpace>::space_ = nullptr;

struct ManhattanHeuristic {
    template <class ExplicitSpace>
    typename ExplicitSpace::CostType
    operator()(const ExplicitState<ExplicitSpace> &s,
               const ExplicitState<ExplicitSpace> &goal) const {
        return ExplicitState<ExplicitSpace>::space()->manhattan(s.raw(),
                                                                goal.raw());
    }
};

struct OctileHeuristic {
    template <class ExplicitSpace>
    typename ExplicitSpace::CostType
    operator()(const ExplicitState<ExplicitSpace> &s,
               const ExplicitState<ExplicitSpace> &goal) const {
        return ExplicitState<ExplicitSpace>::space()->octile(s.raw(),
                                                             goal.raw());
    }
};

#endif
