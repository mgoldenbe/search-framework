#ifndef TABLE_H
#define TABLE_H

#include "utilities.h"

struct Row {
    Row(std::vector<int> &widths, const int &space)
        : widths_(widths), space_(space) {}
    Row &operator=(const Row &rhs) {
        cells_ = rhs.cells_;
        widths_ = rhs.widths_;
        return *this;
    }
    void add(decltype(std::setw(0))) {}

    template <typename T> void add(const T &t) {
        std::string myStr = trim(str(t));
        cells_.push_back(str(t));
    }

    void reset() { cells_.clear(); }
    int size() { return cells_.size(); }

    template <typename TChar, typename TCharTraits>
    friend std::basic_ostream<TChar, TCharTraits> &operator<<(
        std::basic_ostream<TChar, TCharTraits> &o, const Row &r) {
        for (int i = 0; i != r.cells_.size(); ++i)
            o << std::setw(r.widths_[i] + r.space_) << r.cells_[i];
        return o;
    }
private:
    std::vector<std::string> cells_;
    std::vector<int> &widths_;
    const int &space_;
};

struct Table {
    Table(int space) : space_(space), currentRow_(widths_, space_) {}

    const Row &row(int n) const { return rows_[n]; }

    int size() const { return rows_.size(); }

    void clear() {
        rows_.clear();
        widths_.clear();
        currentRow_.reset();
    }

    void remove(int n) {(void)n;
        rows_.erase(rows_.begin() + n);
    }

    template <typename T> friend Table &operator<<(Table &t, const T &el) {
        auto &row = t.currentRow_;
        auto len = str(el).size();
        row.add(el);
        if (row.size() > t.widths_.size()) t.widths_.push_back(len);
        if (row.size())
            t.widths_[row.size() - 1] =
                std::max(t.widths_[row.size() - 1], (int)len);
        return t;
    }

    friend Table &operator<<(
        Table &t, decltype(std::endl<char, std::char_traits<char>>)) {
        t.rows_.push_back(t.currentRow_);
        t.currentRow_.reset();
        return t;
    }

    template <typename TChar, typename TCharTraits>
    friend std::basic_ostream<TChar, TCharTraits> &
    operator<<(std::basic_ostream<TChar, TCharTraits> &o, const Table &t) {
        for (auto &r : t.rows_) o << r << std::endl;
        return o;
    }

private:
    std::vector<Row> rows_;
    std::vector<int> widths_;
    int space_;
    Row currentRow_;
};

void testTable() {
    Table t{1};
    t << "aa" << "bbbb" << std::setw(10) << "d" << std::endl;
    t << "aaa" << "bbbbb" << "dd" << std::endl;
    t << "a" << "bb" << std::endl;
    t << "aaa" << "bbbbb" << "dd" << "cccc" << std::endl;
    std::cout << t;
    std::cout << t.row(2) << std::endl;
}

#endif
