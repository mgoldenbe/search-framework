#ifndef INSTANCE_MEASURES_H
#define INSTANCE_MEASURES_H

/// \file
/// \brief Implementation of instance measures.
/// \author Meir Goldenberg

namespace InstanceMeasures {

/// One instance measure -- the cost.
struct Base {
    /// Call operator to compute the measures.
    /// \tparam Instance The instance type.
    /// \param instance The instance.
    /// \return The measures of \c instance.
    template <class Instance>
    MeasureSet operator()(const Instance &instance);
};

/// One additional instance measure -- the distance between the first two goals.
struct CostGoal0Goal1 : Base {
    /// Call operator to compute the measures.
    /// \tparam Instance The instance type.
    /// \param instance The instance.
    /// \return The measures of \c instance.
    template <class Instance> MeasureSet operator()(const Instance &instance);
};

}

#endif
