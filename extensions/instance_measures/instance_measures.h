#ifndef INSTANCE_MEASURES_H
#define INSTANCE_MEASURES_H

/// \file
/// \brief Implementation of instance measures.
/// \author Meir Goldenberg

/// No measures.
struct Nothing {
    /// Call operator to compute the measures.
    /// \tparam Instance The instance type.
    /// \return The measures of \c instance.
    template <class Instance> MeasureSet operator()(const Instance &) {
        return MeasureSet{};
    }
};

/// One instance measure -- the cost.
struct Base {
    /// Call operator to compute the measures.
    /// \tparam Instance The instance type.
    /// \param instance The instance.
    /// \return The measures of \c instance.
    template <class Instance>
    MeasureSet operator()(const Instance &instance) {
        MeasureSet res{};
        SLB_ALGORITHM<false> alg(instance);
        res.append(Measure("I. Cost", alg.run()));
        return res;
    }
};

/// One additional instance measure -- the distance between the first two goals.
struct CostGoal0Goal1 : Base {
    /// Call operator to compute the measures.
    /// \tparam Instance The instance type.
    /// \param instance The instance.
    /// \return The measures of \c instance.
    template <class Instance> MeasureSet operator()(const Instance &instance) {
        using State = typename Instance::State;

        auto res = static_cast<Base *>(this)->operator()(instance);
        auto goals = instance.goals();
        assert(goals.size() >= 2);
        Instance myInstance(std::vector<State>{goals[0]},
                            std::vector<State>{goals[1]}, MeasureSet{});
        SLB_ALGORITHM<false> alg(myInstance);
        res.append(Measure("G0-G1", alg.run()));
        return res;
    }
};

#endif
