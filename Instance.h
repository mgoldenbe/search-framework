#ifndef INSTANCE_H
#define INSTANCE_H

#include "utilities.h"
#include "Table.h"

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

template <class State> struct SingleStartSingleGoal {
    SingleStartSingleGoal() : start{State::random()}, goal{State::random()} {}
    SingleStartSingleGoal(const std::string &str) {
        std::string mystr = str;
        start = State(stuff(mystr));
        goal = State(stuff(mystr));
    }

    template <class Stream> Stream &dump(Stream &os) const {
        os << start << goal;
        return os;
    }
    template <class Stream> static Stream &dumpTitle(Stream &os) {
        os << "start" << "goal";
        return os;
    }
    State start, goal;
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
