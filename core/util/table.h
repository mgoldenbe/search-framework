#ifndef SLB_CORE_UTIL_TABLE_H
#define SLB_CORE_UTIL_TABLE_H

///@file
///@brief The \ref slb::core::util::Table class.
/// \author Meir Goldenberg

namespace slb {
namespace core {
namespace util {

/// The row of the \ref Table.
struct Row {
    /// Constructor based on the widths of the columns and amount of space
    /// between columns. Note that both arguments are passed by reference,
    /// so we do not keep a copy for each row.
    /// \param widths The widths of the columns.
    /// \param space Amount of space between columns.
    Row(std::vector<int> &widths, const int &space)
        : widths_(widths), space_(space) {}

    /// The assignment operator.
    /// \param rhs The right-hand side of the assignment.
    /// \return The assigned row.
    Row &operator=(const Row &rhs) {
        cells_ = rhs.cells_;
        widths_ = rhs.widths_;
        return *this;
    }

    /// An auxiliary function to make it possible to use \c std::setw with the
    /// output operator whose left-hand side is \ref Table.
    void add(decltype(std::setw(0))) {}

    /// Add a column.
    /// t The column's contents.
    template <typename T> void add(const T &t) {
        std::string myStr = trim(str(t));
        cells_.push_back(str(t));
    }

    /// Clear the row's contents.
    void reset() { cells_.clear(); }

    /// Return the number of columns in the row.
    /// \return The number of columns in the row.
    int size() { return cells_.size(); }

    /// The output operator.
    /// \tparam TChar The \c TChar parameter of \c std::basic_ostream.
    /// \tparam TCharTraits The \c TCharTraits parameter of \c
    /// std::basic_ostream.
    /// \param o The output stream.
    /// \param r The row to be output.
    /// \return The updated output stream.
    template <typename TChar, typename TCharTraits>
    friend std::basic_ostream<TChar, TCharTraits> &operator<<(
        std::basic_ostream<TChar, TCharTraits> &o, const Row &r) {
        for (int i = 0; i != r.cells_.size(); ++i)
            o << std::setw(r.widths_[i] + r.space_) << r.cells_[i];
        return o;
    }
private:
    std::vector<std::string> cells_; ///< The cells.
    std::vector<int> &widths_; ///< Reference to the widths of the columns.
    const int &space_; ///< Reference to the amount of space between the columns.
};

/// The class for handling tabular output, where the columns need to be aligned.
/// The user does not need to determine the width of columns in advance.
struct Table {
    /// The constructor based on the desired amount of space between columns.
    Table(int space = 2) : space_(space), currentRow_(widths_, space_) {}

    /// Returns the row with the given number.
    /// \param n The row number (rows are numbered from zero).
    /// \return Const reference to the row with the given number.
    const Row &row(int n) const { return rows_[n]; }

    /// Returns the number of rows in the table.
    /// \return The number of rows in the table.
    int size() const { return rows_.size(); }

    /// Clears the table.
    void clear() {
        rows_.clear();
        widths_.clear();
        currentRow_.reset();
    }

    /// Removes the row with the given number.
    /// \param n The row number (rows are numbered from zero).
    void remove(int n) {(void)n;
        rows_.erase(rows_.begin() + n);
    }

    /// Precludes the table title from being output during the next output
    /// operation.
    void hideTitle() { hideTitle_ = true; }

    /// Causes the table title from being output during the next output
    /// operation.
    void showTitle() { hideTitle_ = false; }

    /// Reads an object into the table. The string representation of the object
    /// is appended as a column in the currently last row.
    /// \tparam T The type of the object.
    /// t The \ref Table.
    /// el The object to be read.
    /// \return The updated table.
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

    /// Reads \c std::endl into the table, which results in closing the current
    /// row and adding it into the table.
    /// \return The updated table.
    friend Table &operator<<(
        Table &t, decltype(std::endl<char, std::char_traits<char>>)) {
        t.rows_.push_back(t.currentRow_);
        t.currentRow_.reset();
        return t;
    }

    /// The output operator.
    /// \tparam TChar The \c TChar parameter of \c std::basic_ostream.
    /// \tparam TCharTraits The \c TCharTraits parameter of \c
    /// std::basic_ostream.
    /// \param o The output stream.
    /// \param t The table to be output.
    /// \return The updated output stream.
    template <typename TChar, typename TCharTraits>
    friend std::basic_ostream<TChar, TCharTraits> &
    operator<<(std::basic_ostream<TChar, TCharTraits> &o, const Table &t) {
        for (int i = (t.hideTitle_ ? 1 : 0); i != t.rows_.size(); i++)
            o << t.rows_[i] << std::endl;
        return o;
    }

private:
    std::vector<Row> rows_; ///< The rows of the table.
    std::vector<int> widths_; ///< The width of the columns.
    int space_; ///< The amount of space between the columns.
    Row currentRow_; ///< The row currently being filled out.

    /// The flag showing whether the title row needs to be shown during the next
    /// operation of the output operator.
    bool hideTitle_ = false;
};

/// A function to test \ref Table.
void testTable() {
    Table t{1};
    t << "aa" << "bbbb" << std::setw(10) << "d" << std::endl;
    t << "aaa" << "bbbbb" << "dd" << std::endl;
    t << "a" << "bb" << std::endl;
    t << "aaa" << "bbbbb" << "dd" << "cccc" << std::endl;
    std::cout << t;
    std::cout << t.row(2) << std::endl;
}

} // namespace
} // namespace
} // namespace

#endif
