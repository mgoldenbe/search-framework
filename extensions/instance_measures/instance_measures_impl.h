#ifndef INSTANCE_MEASURES_IMPLH
#define INSTANCE_MEASURES_IMPL_H

/// \file
/// \brief Implementation of instance measures.
/// \author Meir Goldenberg

namespace InstanceMeasures {

template <class Instance>
MeasureSet Base::operator()(const Instance &instance) {
    MeasureSet res{};
    SLB_ALGORITHM<false> alg(instance);
    res.append(Measure("I. Cost", alg.run()));
    return res;
}

template <class Instance>
MeasureSet CostGoal0Goal1::operator()(const Instance &instance) {
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

}

#endif
