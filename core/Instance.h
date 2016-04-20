#ifndef INSTANCE_H
#define INSTANCE_H

// Look for the first substring that begins with a non-space character
// and ends either at the end of string or with nSpaces spaces.
// The original string is changed for the non-parsed part of the string.
std::string stuff(std::string &s, bool throwOnEmpty = false, int nSpaces = 2) {
    std::string res;
    auto begin = s.find_first_not_of(" ");
    std::string space = std::string((unsigned)nSpaces, ' ');
    auto end = std::min(s.size(), s.find(space, begin));
    res = std::string{s.begin() + begin, s.begin() + end};
    s = std::string(s.begin() + end, s.end());
    if (throwOnEmpty && res == "")
        throw std::runtime_error("Instance file ill-formed");
    return res;
}

void getNStartsGoals(std::string &s, int &nStarts, int &nGoals) {
    bool startsFlag = true;
    nStarts = nGoals = 0;
    std::string token = stuff(s);
    if (token != "#") throw std::runtime_error("Instance file ill-formed");
    while (s != "") {
        std::string token = stuff(s);
        transform(token.begin(), token.end(), token.begin(), tolower);
        if (token.find("start") == 0) {
            if (!startsFlag)
                throw std::runtime_error("Instance file ill-formed");
            ++nStarts;
        }
        else if (token.find("goal") == 0) {
            if (startsFlag) startsFlag = false;
            ++nGoals;
        }
        else throw std::runtime_error("Instance file ill-formed");
    }
}

template <class State_>
struct Instance {
    using State = State_;
    using MyType = Instance<State>;

    Instance(const std::vector<State> &start, const std::vector<State> &goal)
        : start_(start), goal_(goal) {}

    State &start() { return start_[0]; }
    State &goal() { return goal_[0]; }
    std::vector<State> &goals() { return goal_; }

    std::vector<MyType> goalInstances() const {
        std::vector<MyType> res;
        for (auto g: goal_)
            res.push_back(MyType(start_, std::vector<State>{g}));
        return res;
    }

    template <class Stream> Stream &dump(Stream &os) const {
        for (auto s: start_) os << s;
        for (auto s: goal_) os << s;
        return os;
    }
    template <class Stream> Stream &dumpTitle(Stream &os) {
        for (int i = 0; i != start_.size(); ++i) os << "start-" + str(i);
        for (int i = 0; i != goal_.size(); ++i) os << "goal-" + str(i);
        return os;
    }

private:
    std::vector<State> start_, goal_;
};

// get a random state that is not in the vector
template <class State>
State uniqueRandomState(const std::vector<State> &v) {
    State res;
    while (true) {
        res = State::random();
        if (std::find(v.begin(), v.end(), res) == v.end()) break;
    }
    return res;
}

template <class State>
std::vector<Instance<State>> makeInstances(int n) {
    using MyInstance = Instance<State>;
    std::vector<MyInstance> res;
    int nStarts = CMD.nStarts('w');
    int nGoals = CMD.nGoals('w');
    for (int i = 0; i < n; i++) {
        std::vector<State> start;
        std::vector<State> goal;
        for (int i = 0; i != nStarts; i++)
            start.push_back(uniqueRandomState(start));
        for (int i = 0; i != nGoals; i++)
            goal.push_back(uniqueRandomState(goal));
        res.push_back(MyInstance(start, goal));
    }
    return res;
}

template <class State = SLB_STATE>
std::vector<Instance<State>> makeInstancesFile(const std::string &fname) {
    using MyInstance = Instance<State>;
    Table t;
    std::ofstream fs{fname};
    if (!fs) throw std::invalid_argument("Could not create the instances file");
    if (CMD.nInstances() < 1)
        throw std::invalid_argument("Can't be fewer than one instance");
    std::vector<MyInstance> res = makeInstances<State>(CMD.nInstances());

    t << "#";
    res[0].dumpTitle(t);
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

template <class State>
std::vector<Instance<State>> readInstancesFile(const std::string &fname) {
    using MyInstance = Instance<State>;
    std::ifstream fs{fname};
    if (!fs) throw std::invalid_argument("Could not open the instances file");
    std::vector<MyInstance> res;

    std::string line;

    // Title line
    int totalNStarts, totalNGoals;
    std::getline(fs, line);
    getNStartsGoals(line, totalNStarts, totalNGoals);

    int nStarts = std::min(totalNStarts, CMD.nStarts('r'));
    int nGoals = std::min(totalNGoals, CMD.nGoals('r'));

    // Instances
    while (std::getline(fs, line)) {
        std::vector<State> start, goal;
        std::string startLine, goalLine;

        stuff(line, true); // skip instance number
        for (int i = 0; i != nStarts; ++i)
            start.push_back(State(stuff(line, true)));
        for (int i = nStarts; i != totalNStarts; ++i)
            stuff(line, true); // skip not used starts
        for (int i = 0; i != nGoals; ++i)
            goal.push_back(State(stuff(line, true)));
        res.push_back(MyInstance(start, goal));
    }
    return res;
}

#endif
