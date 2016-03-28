///@file
///@brief INTERFACES CHECKED.

#ifndef MANAGED_NODE
#define MANAGED_NODE

#include "Node2Stream.h"
#include "NodesDiff.h"

template <class Base1_ = void, class Base2_ = void, class Base3_ = void,
          class Base4_ = void>
struct ManagedNode;

// For classes that do not derive
template<> struct ManagedNode<void, void, void, void> {
    using Base1 = void;
    REFLECTABLE()
};

// For classes that derive from a single base class
template <class Base1_>
struct ManagedNode<Base1_, void, void, void> : public Base1_ {
    using Base1 = Base1_;
    using Base2 = void;
    REFLECTABLE()
};

// For classes that derive from two base classes
template <class Base1_, class Base2_>
struct ManagedNode<Base1_, Base2_, void, void>
    : public Base1_, public Base2_ {
    using Base1 = Base1_;
    using Base2 = Base2_;
    using Base3 = void;
    REFLECTABLE()
};

#endif
