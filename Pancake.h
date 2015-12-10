///@file
///@brief INTERFACES CHECKED.

#ifndef PANCAKE
#define PANCAKE

#include <iostream>
#include <vector>
#include <boost/functional/hash.hpp>
#include "PrettyPrint.h"
#include "Node.h"
#include "utilities.h"

struct Pancake {
    using CostType = int; // required by the library
    using Neighbor = StateNeighbor<Pancake>;

    ///@name Construction and Assignment
    //@{
    Pancake(int n) : pancakes(n) {
        for (auto i = 0; i != n; ++i)
            pancakes[i] = i;
    }
    Pancake (const Pancake &p) = default;
    Pancake &operator=(const Pancake &rhs) = default;
    //@}

    ///@name Read-Only Services
    //@{
    const std::vector<int> &getPancakes() const { return pancakes; }

    Pancake *move(int pos) {
        std::reverse(pancakes.begin(), pancakes.begin() + pos + 1);
        return this;
    }

    std::vector<Neighbor> successors() const {
        std::vector<Neighbor> res;
        for (auto i = 1U; i != pancakes.size(); ++i) {
            Neighbor cur((new Pancake(*this)) -> move(i));
            res.push_back(std::move(cur));
        }
        return res;
    }

    int gapHeuristic() const {
        int res = 0;
        for (unsigned i = 0U; i < pancakes.size() - 1; i++)
            if (abs(pancakes[i] - pancakes[i + 1]) > 1) res++;
        if (pancakes.back() != (int)pancakes.size() - 1) res++;
        return res;
    }

    int gapHeuristic(const Pancake &goal) const {
        Pancake temp = *this;
        std::vector<int> transform(goal.pancakes.size());
        for (auto i = 0U; i < goal.pancakes.size(); ++i)
            transform[goal.pancakes[i]] = i;
        for (auto i = 0U; i < temp.pancakes.size(); ++i)
            temp.pancakes[i] = transform[temp.pancakes[i]];
        return temp.gapHeuristic();
    }

    std::size_t hash() const {
        boost::hash<std::vector<int>> v_hash;
        return v_hash(pancakes);
    }

    template <typename charT>
    std::basic_ostream<charT> &dump(std::basic_ostream<charT> &o) const {
        return o << pancakes;
    }
    //@}


    ///@name Modification
    //@{
    void shuffle() {
        auto old = pancakes;
        while (old == pancakes)
            std::random_shuffle(pancakes.begin(), pancakes.end());
    }
    //@}

    friend bool operator== (const Pancake &p1, const Pancake &p2);

private:
    std::vector<int> pancakes;
};

void dump(const Pancake &p) { dumpT(p); }

bool operator==(const Pancake &p1, const Pancake &p2) {
    return p1.pancakes == p2.pancakes;
}

bool operator==(const std::shared_ptr<const Pancake> &p1,
                const std::shared_ptr<const Pancake> &p2) {
    return *p1 == *p2;
}

//------------------------- HEURISTICS ------------------//

struct GapHeuristic {
    int operator()(const Pancake &s) const { return s.gapHeuristic(); }
};

struct GapHeuristicToGoal {
    int operator()(const Pancake &s, const Pancake &goal) const {
        return s.gapHeuristic(goal);
    }
};

#endif
