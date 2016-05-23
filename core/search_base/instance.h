#ifndef INSTANCE_H
#define INSTANCE_H

/// \file
/// \brief Facilities for working with problem instances.
/// \author Meir Goldenberg

/// Looks for the first substring that begins with a non-space character
/// and ends either at the end of string or with the given number of spaces.
/// The original string is modified to contain the non-parsed part of the
/// string.
/// \param s The string.
/// \param throwOnEmpty If \c true and not non-space character is found, an
/// exception is thrown.
/// \param nSpaces Number of spaces between tokens.
/// \return The found token.
/// \note \c s is modified to contain the non-parsed part of the
/// string.
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

/// Parses the given title line of the instances file to determine how many
/// start and goal states each instance would contain.
/// \param s The title line.
/// \param nStarts The number of start states in each instance to be set.
/// \param nGoals The number of goal states in each instance to be set.
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

/// The type for an instance with an arbitrary (but fixed) number of start and
/// goal states.
/// \tparam State The state type, represents the domain.
/// \note Multiple starts are currently not fully handled.
template <class State_>
struct Instance {
    /// The state type, represents the domain.
    using State = State_;

    /// The instance type.
    using MyType = Instance<State>;

    /// Initializes the instance based on the given start and goal states.
    /// \param start Vector of start states.
    /// \param goal Vector of goal states.
    Instance(const std::vector<State> &start, const std::vector<State> &goal)
        : start_(start), goal_(goal) {}

    /// Returns the first start state. Intended for use when the instance
    /// contains a single start state.
    /// \return Const reference to the start state.
    const State &start() const { return start_[0]; }

    /// Returns the first goal state. Intended for use when the instance
    /// contains a single goal state.
    /// \return Const reference to the goal state.
    const State &goal() const { return goal_[0]; }

    /// Returns the goal states.
    /// \return Non-const reference to the vector of goal states.
    /// \note The reference in the return value is non-const, since some search
    /// algorithms need to modify the set of goals as the algorithm progresses.
    /// See \ref MinHeuristicGoalHandler for an example.
    std::vector<State> &goals() { return goal_; }

    /// Returns a vector of single-goal instances, one instance for each goal
    /// state.
    /// \return Vector of single-goal instances, one instance for each goal
    /// state.
    std::vector<MyType> goalInstances() const {
        std::vector<MyType> res;
        for (auto g: goal_)
            res.push_back(MyType(start_, std::vector<State>{g}));
        return res;
    }

    /// Dumps the instance to the given stream.
    /// \tparam Stream The stream type.
    /// \param os The stream.
    template <class Stream> Stream &dump(Stream &os) const {
        for (auto s: start_) os << s;
        for (auto s: goal_) os << s;
        return os;
    }

    /// Dumps the line with column titles to the given stream.
    /// \tparam Stream The stream type.
    /// \param os The stream.
    template <class Stream> Stream &dumpTitle(Stream &os) {
        for (int i = 0; i != start_.size(); ++i) os << "start-" + str(i);
        for (int i = 0; i != goal_.size(); ++i) os << "goal-" + str(i);
        return os;
    }

private:
    std::vector<State> start_; ///< The start states.
    std::vector<State> goal_;  ///< The goal states.
};

/// Computes a random state that is not in the given vector of states.
/// \tparam State The state type, represents the domain.
/// \param v The given vector of states.
/// \return A random state that is not in \c v.
template <class State>
State uniqueRandomState(const std::vector<State> &v) {
    State res;
    while (true) {
        res = State::random();
        if (std::find(v.begin(), v.end(), res) == v.end()) break;
    }
    return res;
}

/// Makes the required number of random instances. The numbers of start and goal
/// states are determined by the command line.
/// \tparam State The state type, represents the domain.
/// \param n The required number of instances.
/// \return Vector of \c n random instances.
/// \warning If the number of different states in the domain is smaller than the
/// number of start or goal states in an instance), an infinite loop will
/// result.
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

/// Makes a file with the number of random instances determined by the command
/// line. The numbers of start and goal states are determined by the command
/// line as well.
/// \tparam State The state type, represents the domain.
/// \param fname The name of the file to contain the instances.
/// \return Vector of the instances stored in the file.
/// \warning If the number of different states in the domain is smaller than the
/// number of start or goal states in an instance), an infinite loop will
/// result.
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

/// Reads the instances file.
/// \tparam State The state type, represents the domain.
/// \param fname The name of the file containing the instances.
/// \return Vector of the read instances.
/// \note The numbers of start and goal states in the instances are extracted
/// from the file. The number of instances is determined automatically as well.
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
