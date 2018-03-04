#ifndef SLB_EXTENSIONS_INSTANCE_MEASURES_INSTANCE_MEASURES_H
#define SLB_EXTENSIONS_INSTANCE_MEASURES_INSTANCE_MEASURES_H

/// \file
/// \brief Implementation of instance measures.
/// \author Meir Goldenberg

namespace slb {
namespace ext {
namespace instanceMeasure {

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
        using State = typename Instance::State;
        double sum = 0;
        MeasureSet res{};
        auto starts = instance.starts();
        auto goals = instance.goals();
        for (auto i = 0U; i < goals.size(); ++i) {
            Instance myInstance(starts, std::vector<State>{goals[i]},
                                MeasureSet{});
            SLB_ALGORITHM<false> alg(myInstance);
            sum += alg.run();
            res.append(
                Measure("Cost=Goals-" + std::to_string(i + 1), sum / (i + 1)));
        }
        return res;
    }
};

/// A radius for each each additional goal.
/// So, R1 -- radius for 2 goals etc.
struct Radius : Base {
    /// Call operator to compute the measures.
    /// \tparam Instance The instance type.
    /// \param instance The instance.
    /// \return The measures of \c instance.
    template <class Instance> MeasureSet operator()(const Instance &instance) {
        using State = typename Instance::State;
        using namespace std;
        using CostType = SLB_ALGORITHM<false>::CostType;
        MeasureSet res{};
        auto goals = instance.goals();
        assert(goals.size() >= 2);

        // Add the base measure.
        res.append(static_cast<Base *>(this)->operator()(instance));

        CostType radius = (CostType)0;
        for (auto i = 1U; i < goals.size(); ++i) {
            Instance myInstance(std::vector<State>{goals[0]},
                                std::vector<State>{goals[i]}, MeasureSet{});
            SLB_ALGORITHM<false> alg(myInstance);
            CostType cur = alg.run();
            radius = std::max(radius, cur);
            res.append(
                Measure("Radius=Goals-" + std::to_string(i + 1), radius));
        }
        return res;
    }
};

} // namespace
} // namespace
} // namespace

#endif
