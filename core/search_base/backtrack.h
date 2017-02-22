#ifndef SLB_CORE_SEARCH_BASE_BACKTRACK_H
#define SLB_CORE_SEARCH_BASE_BACKTRACK_H

/// \file
/// \brief The class for handling backtracking in IDA*.
/// \author Meir Goldenberg

namespace slb {
namespace core {
namespace sb {

/// Handles backtracking in linear-space algorithms such as IDA*.
/// \tparam Algorithm The search algorithm.
/// \tparam Lock_ The RAII class for handling backtracking.
template <class MyAlgorithm, template <class, bool> class Lock_>
struct Backtrack {
    /// The RAII class for handling backtracking.
    using Lock = Lock_<MyAlgorithm, MyAlgorithm::logFlag>;

    /// The constructor.
    Backtrack(MyAlgorithm &alg) : alg_(alg) {
        if (!Lock::keepsParent) {
            // so dynamic heuristic would look up h-value of parent correctly.
            // Lock may set up parent differently if needed.
            auto cur = alg.cur();
            cur->setParent(cur);
        }
    }

    /// Makes the backtrack RAII object for a neighbor.
    /// \tparam Neighbor The neighbor type.
    /// \param n The neighbor.
    /// \return The backtrack RAII object.
    template <class Neighbor>
    Lock lock(Neighbor &n) { return Lock(alg_, n); }

private:
    /// Reference to the search algorithm.
    MyAlgorithm &alg_;
};

} // namespace
} // namespace
} // namespace

#endif
