#ifndef INSTANCE_H
#define INSTANCE_H

/// \file
/// \brief Facilities for working with problem instances.
/// \author Meir Goldenberg

#include "stats.h"

namespace slb {
namespace core {
namespace sb {

using util::stuff;
using util::str;
using util::Table;

/// Parses the given title line of the instances file to determine how many
/// start and goal states each instance would contain. Returns a measure set
/// with the titles already set.
/// \param s The title line.
/// \param nStarts The number of start states in each instance to be set.
/// \param nGoals The number of goal states in each instance to be set.
MeasureSet parseInstancesTitle(std::string &s, int &nStarts, int &nGoals) {
    MeasureSet res{};
    bool startsFlag = true;
    nStarts = nGoals = 0;
    std::string token = stuff(s);
    if (token != "#") throw std::runtime_error("Instance file ill-formed");
    while ((token = stuff(s)) != "") {
        std::string origToken = token;
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
        else
            res.append(Measure{origToken});
    }
    return res;
}

/// The type for an instance with an arbitrary (but fixed) number of start and
/// goal states.
/// \tparam State The state type, represents the domain.
/// \note Multiple starts are currently not fully handled.
template <class State_>
struct Instance {
    /// The state type, represents the domain.
    using State = State_;

    /// Initializes the instance based on the given start and goal states.
    /// \tparam InstanceMeasures The functor for computing the instance measures.
    /// \param start Vector of start states.
    /// \param goal Vector of goal states.
    template <
        typename InstanceMeasures = SLB_INSTANCE_MEASURES>
    Instance(const std::vector<State> &start, const std::vector<State> &goal)
        : start_(start), goal_(goal), measures_(InstanceMeasures{}(*this)) {}

    /// Initializes the instance based on the given start and goal states and a
    /// set of measures.
    /// \param start Vector of start states.
    /// \param goal Vector of goal states.
    /// \param measures The set of measures
    Instance(const std::vector<State> &start, const std::vector<State> &goal,
             const MeasureSet &measures)
        : start_(start), goal_(goal), measures_(measures) {}

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
    /// See \ref ext::policy::goalHandler::MinHeuristic for an example.
    const std::vector<State> &goals() const { return goal_; }

    /// Returns a vector of single-goal instances, one instance for each goal
    /// state.
    /// \return Vector of single-goal instances, one instance for each goal
    /// state.
    std::vector<Instance> goalInstances() const {
        std::vector<Instance> res;
        for (auto g: goal_)
            res.push_back(Instance{start_, std::vector<State>{g}, MeasureSet{}});
        return res;
    }

    /// Dumps the instance to the given stream.
    /// \tparam Stream The stream type.
    /// \param os The stream.
    template <class Stream> Stream &dump(Stream &os) const {
        for (auto s: start_) os << s;
        for (auto s: goal_) os << s;
        return measures_.dumpValues(os);
    }

    /// Dumps the line with column titles to the given stream.
    /// \tparam Stream The stream type.
    /// \param os The stream.
    template <class Stream> Stream &dumpTitle(Stream &os) {
        for (int i = 0; i != start_.size(); ++i) os << "start-" + str(i);
        for (int i = 0; i != goal_.size(); ++i) os << "goal-" + str(i);
        return measures_.dumpTitle(os);
    }

    /// Returns the measure set of the instance.
    /// \return The measure set of the instance.
    const MeasureSet &measures() const { return measures_; }

private:
    std::vector<State> start_; ///< The start states.
    std::vector<State> goal_;  ///< The goal states.
    MeasureSet measures_; ///< The measures associated with the instance.
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
template <class State, CMD_TPARAM>
std::vector<Instance<State>> makeInstances(int n) {
    using MyInstance = Instance<State>;
    std::vector<MyInstance> res;
    int nStarts = CMD_T.nStarts('w');
    int nGoals = CMD_T.nGoals('w');
    for (int i = 0; i < n; i++) {
        std::vector<State> start;
        std::vector<State> goal;
        for (int i = 0; i != nStarts; i++)
            start.push_back(uniqueRandomState(start));
        for (int i = 0; i != nGoals; i++)
            if (i == 0 && CMD_T.defaultGoal())
                goal.push_back(State{});
            else
                goal.push_back(uniqueRandomState(goal));
        res.push_back(MyInstance(start, goal));
    }
    if (res[0].measures().size()) // otherwise, no measure to sort on
        std::sort(res.begin(), res.end(), [](const MyInstance &i1,
                                             const MyInstance &i2) {
            return i1.measures()[0].value() < i2.measures()[0].value();
        });
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
template <class State, CMD_TPARAM>
std::vector<Instance<State>> makeInstancesFile(const std::string &fname) {
    using MyInstance = Instance<State>;
    Table t;
    std::ofstream fs{fname};
    if (!fs) throw std::invalid_argument("Could not create the instances file");
    if (CMD_T.nInstances() < 1)
        throw std::invalid_argument("Can't be fewer than one instance");
    std::vector<MyInstance> res = makeInstances<State>(CMD_T.nInstances());

    t << "#";
    res[0].dumpTitle(t);

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
template <class State, CMD_TPARAM>
std::vector<Instance<State>> readInstancesFile(const std::string &fname) {
    using MyInstance = Instance<State>;
    std::ifstream fs{fname};
    if (!fs) throw std::invalid_argument("Could not open the instances file");
    std::vector<MyInstance> res;

    std::string line;

    // Title line
    int totalNStarts, totalNGoals;
    std::getline(fs, line);
    MeasureSet measures = parseInstancesTitle(line, totalNStarts, totalNGoals);

    int nStarts = std::min(totalNStarts, CMD_T.nStarts('r'));
    int nGoals = std::min(totalNGoals, CMD_T.nGoals('r'));

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
        for (int i = nGoals; i != totalNGoals; ++i)
            stuff(line, true); // skip not used goals
        for (auto &m: measures) {
            double x = -1;
            istringstream{stuff(line, true)} >> x;
            assert(x != -1);
            m.set(x);
        }
        res.push_back(MyInstance(start, goal, measures));
    }
    return res;
}

} // namespace
} // namespace
} // namespace

#endif
