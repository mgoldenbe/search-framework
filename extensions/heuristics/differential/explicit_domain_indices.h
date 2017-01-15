#ifndef SLB_EXTENSIONS_HEURISTICS_DIFFERENTIAL_EXPLICIT_DOMAIN_INDICES_H
#define SLB_EXTENSIONS_HEURISTICS_DIFFERENTIAL_EXPLICIT_DOMAIN_INDICES_H

/// \file
/// \brief The indices for an explicit domain.
/// \author Meir Goldenberg

namespace slb {
namespace ext {
namespace domain {

/// The index with inverse for domains whose states are integers (or a type
/// convertible to int), so that the state itself can be used as the index. Note
/// that these integers do not have to be consecutive, i.e. a map can have
/// blocks.
template <class ExplicitState>
struct TrivialIndex {
    static constexpr auto kind = IndexKind::withInverse;

    static int to(const ExplicitState &s) {
        return static_cast<int>(s.raw());
    }

    static ExplicitState from(int index) {
        return ExplicitState{
            static_cast<typename ExplicitState::Location>(index)};
    }

    static int size() {
        return ExplicitState::space()->rawSize();
    }
};

} // namespace
} // namespace
} // namespace

#endif
