#ifndef INSTANCE_H
#define INSTANCE_H

// Look for the first substring that begins with a non-space character
// and ends either at the end of string or with nSpaces spaces.
// The original string is changed for the non-parsed part of the string.
std::string stuff(std::string &s, int nSpaces = 2) {
    std::string res;
    auto begin = s.find_first_not_of(" ");
    std::string space = std::string((unsigned)nSpaces, ' ');
    auto end = std::min(s.size(), s.find(space, begin));
    res = std::string{s.begin() + begin, s.begin() + end};
    s = std::string(s.begin() + end, s.end());
    return res;
}

template <class State_, bool goalFlag> struct SingleStartGoalState {
    using State = State_;

    SingleStartGoalState() : state_{State::random()} {}
    SingleStartGoalState(const State &state) : state_(state) {}
    SingleStartGoalState(std::string &str) {
        state_ = State(stuff(str));
    }

    template <class Stream> Stream &dump(Stream &os) const {
        os << state_;
        return os;
    }
    template <class Stream> static Stream &dumpTitle(Stream &os) {
        os << (goalFlag ? "goal" : "start");
        return os;
    }
    State state_;
};
template <class State = STATE>
using SingleStartState = SingleStartGoalState<State, false>;
template <class State = STATE>
using SingleGoalState = SingleStartGoalState<State, true>;

template <class State_, int nStates, bool goalFlag>
struct MultipleStartGoalStates {
    using State = State_;

    MultipleStartGoalStates() {
        for (auto &g: states_) g = State::random();
    }
    MultipleStartGoalStates(std::string &str) {
        for (auto &g: states_) g = State(stuff(str));
    }

    template <class Stream> Stream &dump(Stream &os) const {
        for (auto &g: states_) os << g;
        return os;
    }
    template <class Stream> static Stream &dumpTitle(Stream &os) {
        for (int i = 0; i < nStates; i++)
            os << (goalFlag ? "goal-" : "start-") + str(i);
        return os;
    }
    std::vector<State> states_ = std::vector<State>(nStates);
};
template <class State = STATE, int nStates = NSTARTS>
using MultipleStartStates = MultipleStartGoalStates<State, nStates, false>;
template <class State = STATE, int nStates = NGOALS>
using MultipleGoalStates = MultipleStartGoalStates<State, nStates, true>;

template <class Start_ = SEARCH_START, class Goal_ = SEARCH_GOAL>
struct Instance : Start_, Goal_ {
    using Start = Start_;
    using Goal = Goal_;
    using State = typename Start::State;
    using SingleGoal = SingleStartGoalState<State, true>;
    using SingleGoalInstance = Instance<Start, SingleGoal>;

    Instance() : Start(), Goal() {}
    Instance(const Start &s, const Goal &g) : Start(s), Goal(g) {}
    Instance(std::string &str) : Start(str), Goal(str) {}

    std::vector<SingleGoalInstance> goalInstances() const {
        std::vector<SingleGoalInstance> res;

        for (auto g: Goal::states_)
            res.push_back(SingleGoalInstance(*this, g));
        return res;
    }

    template <class Stream> Stream &dump(Stream &os) const {
        Start::dump(os);
        Goal::dump(os);
        return os;
    }
    template <class Stream> static Stream &dumpTitle(Stream &os) {
        Start::dumpTitle(os);
        Goal::dumpTitle(os);
        return os;
    }
};

template <class Instance>
std::vector<Instance> makeInstances(int n) {
    std::vector<Instance> res;
    for (int i = 0; i < n; i++)
        res.push_back(Instance{});
    return res;
}

template <class Instance>
std::vector<Instance> makeInstancesFile(int n, const std::string &fname) {
    Table t;
    std::ofstream fs{fname};
    std::vector<Instance> res = makeInstances<Instance>(n);

    t << "#";
    Instance::dumpTitle(t);
    t << std::endl;

    int i = 0;
    for (auto instance: res) {
        t << i++;
        instance.dump(t);
        t << std::endl;
    }
    fs << t;
    return res;
}

template <class Instance>
std::vector<Instance> readInstancesFile(const std::string &fname) {
    std::ifstream fs{fname};
    if (!fs) throw std::invalid_argument("Could not open the instances file");
    std::vector<Instance> res;

    std::string line;
    std::getline(fs, line);
    while (std::getline(fs, line)) {
        stuff(line); // skip instance number
        res.push_back(Instance(line));
    }

    return res;
}

#endif
