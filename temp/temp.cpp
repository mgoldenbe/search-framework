#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cassert>
#include <functional>
#include <algorithm>
#include "PrettyPrint.h"

std::string sureRead(std::ifstream &fin) {
    std::string res;
    fin >> res;
    assert(fin);
    return res;
}

std::string sureRead(std::ifstream &fin, const std::string &str) {
    std::string res = sureRead(fin);
    assert(res == str);
    return res;
}

struct GridMap {
    GridMap(std::string fileName) {
        std::ifstream fin(fileName);
        assert(fin.is_open());

        sureRead(fin, "type");
        octileFlag_ = (sureRead(fin) == "octile");
        sureRead(fin, "height");
        height_ = std::stoi(sureRead(fin));
        sureRead(fin, "width");
        width_ = std::stoi(sureRead(fin));
        sureRead(fin, "map");

        for (int i = 0; i < height_; i++) {
            std::string str = sureRead(fin);
            assert((int)str.size() == width_);
            for (char c: str)
                v_.push_back(c == '.');
        }
    }

    int location(int r, int c) { return r * width_ + c; }
    int row(int loc) { return loc / width_; }
    int column(int loc) { return loc % width_; }
    bool passable(int r, int c) {
        if (r < 0 || r >= height_) return false;
        if (c < 0 || c >= width_) return false;
        return v_[location(r, c)];
    }
    bool passable(int loc) { return passable(row(loc), column(loc)); }
    std::vector<int> neighbors(int loc) {
        std::vector<int> res;
        int r = row(loc), c = column(loc);
        assert(passable(r, c));

        bool c1, c2;

        if ((c1 = passable(r - 1, c))) res.push_back(location(r - 1, c));

        // Going clock-wise with diagonal directions starting with r-1, c-1
        if ((c2 = passable(r, c - 1))) res.push_back(location(r, c - 1));
        if (octileFlag_ && c1 && c2 && passable(r - 1, c - 1))
                res.push_back(location(r - 1, c - 1));

        if ((c2 = passable(r, c + 1))) res.push_back(location(r, c + 1));
        if (octileFlag_ && c1 && c2 && passable(r - 1, c + 1))
            res.push_back(location(r - 1, c + 1));

        if ((c1 = passable(r + 1, c))) res.push_back(location(r + 1, c));
        if (octileFlag_ && c1 && c2 && passable(r + 1, c + 1))
            res.push_back(location(r + 1, c + 1));

        c2 = passable(r, c - 1);
        if (octileFlag_ && c1 && c2 && passable(r + 1, c - 1))
            res.push_back(location(r + 1, c - 1));

        return res;
    }

private:
    std::vector<bool> v_;
    int width_, height_;
    bool octileFlag_;
};

template<class ExplicitSpace, typename CostType>
struct ExplicitState {
    using MyType = ExplicitState<ExplicitSpace, CostType>;
    using Neighbor = StateNeighbor<MyType>;
    using StateType = typename ExplicitSpace::StateType;

    ///@name Construction and Assignment
    //@{
    ExplicitState(const ExplicitSpace space, const StateType &state)
        : space_(space), state_(state) {}
    ExplicitState(const std::string &s) : state_(space_.state(s)) {}
    ExplicitState(const MyType &) = default;
    ExplicitState &operator=(const MyType &) = default;
    //@}

    ///@name Read-Only Services
    //@{
    std::vector<Neighbor> successors() const {
        std::vector<Neighbor> res;
        for (auto &n: space.neighbors()) {
            Neighbor cur(new StateType(n));
            res.push_back(std::move(cur));
        }
        return res;
    }

    std::size_t hash() const { return boost::hash<StateType>(state_); }
    //@}

    template <typename charT>
    friend std::basic_ostream<charT> &operator<<(std::basic_ostream<charT> &o,
                                                 const MyType &rhs) {
        return o << rhs.state_;
    }

    friend bool operator==(const MyType &lhs, const MyType &rhs) {
        return lhs.state_ == rhs.state_;
    }

private:
    StateType state_;
    const ExplicitSpace &space_;
};

template <class ExplicitSpace, typename CostType>
bool
operator==(const std::shared_ptr<ExplicitState<ExplicitSpace, CostType>> &lhs,
           const std::shared_ptr<ExplicitState<ExplicitSpace, CostType>> &rhs) {
    return *lhs == *rhs;
}

struct ManhattanHeuristic {
    template <class ExplicitSpace, typename CostType>
    CostType
    operator()(const ExplicitState<ExplicitSpace, CostType> &s,
               const ExplicitState<ExplicitSpace, CostType> &goal) const {
        return ExplicitSpace::manhattan(s, goal);
    }
};

int main(void)
{
    GridMap m("tiny1.map8"); (void)m;
    for (auto el: m.neighbors(0))
        std::cout << m.row(el) << " " << m.column(el) << std::endl;
    return (0);
}
