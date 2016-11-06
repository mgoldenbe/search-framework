#ifndef DOMAIN_BASE_H
#define DOMAIN_BASE_H

/// \file
/// \brief The \ref slb::core::sb::DomainBase class.
/// \author Meir Goldenberg

namespace slb {
namespace core {
namespace sb {

/// A type for common features of domains.
struct DomainBase {
    /// Sets the space for this state class by reading it from the given file.
    static void initSpace(const std::string &) {}

    /// Returns the textual label for the vertex representing the state.
    /// \return The textual label for the vertex representing the state.
    std::string visualLabel() const {return "";}

    /// Returns the textual label for the edge to the given state.
    /// \return The textual label for the edge to \c to.
    std::string visualLabel(const DomainBase &) const {return "";}
};

} // namespace
} // namespace
} // namespace

#endif
