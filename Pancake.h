#ifndef PANCAKE
#define PANCAKE

#include <iostream>
#include <vector>
#include <boost/functional/hash.hpp>
#include "PrettyPrint.h"
#include "Node.h"

struct Pancake {
    using CostType = int; // required by the library
    using Neighbor = StateNeighbor<Pancake>;

    Pancake(int n) : pancakes(n) {
        for (auto i = 0; i != n; ++i)
            pancakes[i] = i;
    }
    //Pancake() = default;
    Pancake (const Pancake &p) = default;
    Pancake &operator=(const Pancake &rhs) = default;

    Pancake (Pancake &&p) = default;
    Pancake &operator= (Pancake &&rhs) = default;

    friend bool operator== (const Pancake &p1, const Pancake &p2);
    std::size_t hash() const {
        boost::hash<std::vector<int>> v_hash;
        return v_hash(pancakes);
    }

    void shuffle() {std::random_shuffle(pancakes.begin(), pancakes.end());}
    const std::vector<int> &getPancakes() const {return pancakes;}
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

    std::ostream &dump(std::ostream &o) const { return o << pancakes; }
    void dump() const;

private:
    std::vector<int> pancakes;
};

void Pancake::dump() const { dump(std::cerr) << std::endl; }

std::ostream &operator<<(std::ostream &o, const Pancake &p) {
    return p.dump(o);}
template void dump<Pancake>(const AStarNode<Pancake> &);

bool operator== (const Pancake &p1, const Pancake &p2) {
    return p1.pancakes == p2.pancakes;}

//------------------------- HEURISTICS ------------------//

struct GapHeuristic {
    GapHeuristic(const Pancake &goal) : goal_(goal) {}
private:
    Pancake goal_;
};

#endif
