#ifndef INSTANCE_IMPL_H
#define INSTANCE_IMPL_H

/// \file
/// \brief Dependent implementations taken out from \ref instance.h.
/// \author Meir Goldenberg

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
    std::sort(res.begin(), res.end(),
              [](const MyInstance &i1, const MyInstance &i2) {
                  return i1.measures()[0].value() < i2.measures()[0].value();
              });
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
    MeasureSet measures = parseInstancesTitle(line, totalNStarts, totalNGoals);

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

#endif
