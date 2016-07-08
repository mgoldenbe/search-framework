#ifndef INSTANCE_MEASURES_H
#define INSTANCE_MEASURES_H

/// \file
/// \brief Implementation of instance measures.
/// \author Meir Goldenberg

/// One instance measure -- the cost.
struct Cost {
    /// Call operator to compute the measures.
    /// \tparam Instance The instance type.
    /// \param instance The instance.
    /// \return The measures of \c instance.
    template <class Instance>
    MeasureSet operator()(const Instance &instance);
};

#endif
