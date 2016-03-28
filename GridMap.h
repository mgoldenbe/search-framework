
#ifndef GRID_MAP_H
#define GRID_MAP_H

template <typename CostType_>
struct GridMap {
    using CostType = CostType_;
    using StateType = int;

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

    StateType location(int r, int c) const { return r * width_ + c; }
    int row(StateType loc) const { return loc / width_; }
    int column(StateType loc) const { return loc % width_; }
    bool passable(int r, int c) const {
        if (r < 0 || r >= height_) return false;
        if (c < 0 || c >= width_) return false;
        return v_[location(r, c)];
    }
    bool passable(StateType loc) const {
        return passable(row(loc), column(loc));
    }

    template <class Neighbor>
    void addNeighbor(std::vector<Neighbor> &res, StateType n,
                     bool diagonalFlag = false) const {
        CostType cost = diagonalFlag ? static_cast<CostType>(1.41)
                                     : static_cast<CostType>(1.0);
        res.push_back(Neighbor((new typename Neighbor::State(n)), cost));
    }

    template<class Neighbor>
    std::vector<Neighbor> neighbors(int loc) const {
        std::vector<Neighbor> res;
        int r = row(loc), c = column(loc);
        assert(passable(r, c));

        bool c1, c2;

        if ((c1 = passable(r - 1, c)))
            addNeighbor(res, location(r - 1, c));
            //res.push_back(Neighbor(location(r - 1, c), 1));

        // Going clock-wise with diagonal directions starting with r-1, c-1
        if ((c2 = passable(r, c - 1))) addNeighbor(res, location(r, c - 1));
        if (octileFlag_ && c1 && c2 && passable(r - 1, c - 1))
            addNeighbor(res, location(r - 1, c - 1), true);

        if ((c2 = passable(r, c + 1))) addNeighbor(res, location(r, c + 1));
        if (octileFlag_ && c1 && c2 && passable(r - 1, c + 1))
            addNeighbor(res, location(r - 1, c + 1), true);

        if ((c1 = passable(r + 1, c))) addNeighbor(res, location(r + 1, c));
        if (octileFlag_ && c1 && c2 && passable(r + 1, c + 1))
            addNeighbor(res, location(r + 1, c + 1), true);

        c2 = passable(r, c - 1);
        if (octileFlag_ && c1 && c2 && passable(r + 1, c - 1))
            addNeighbor(res, location(r + 1, c - 1), true);

        return res;
    }

    StateType state(const std::string str) const {
        auto parse = split(str, {' ', ',', '[', ']'});
        assert(parse.size() == 2);
        return location(std::stoi(parse[0]), std::stoi(parse[1]));
    }

    StateType random() const {
        static std::vector<StateType> states;
        if (!states.size()) {
            StateType cur = 0;
            for (auto b : v_) {
                if (b) states.push_back(cur);
                ++cur;
            }
        }
        return states[rand() % states.size()];
    }

    StateType defaultState() const {
        return StateType{-1};
    }

    CostType manhattan(StateType location, StateType goal) const {
        return std::abs(row(location) - row(goal)) +
               std::abs(column(location) - column(goal));
    }

    std::string stateStr(StateType location) const {
        return "[" + str(row(location)) + " " + str(column(location)) + "]";
    }

private:
    std::vector<bool> v_;
    int width_, height_;
    bool octileFlag_;
};

#endif
