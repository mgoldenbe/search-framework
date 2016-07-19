/// \file
/// \brief A base for node with reflection.
/// \author Meir Goldenberg

#ifndef MANAGED_NODE_H
#define MANAGED_NODE_H

namespace Node {

/// Generic declaration to be specified. Currently supporting nodes that inherit
/// data from up to four base classes.
/// \tparam Base1_ First base
/// \tparam Base2_ Second base
/// \tparam Base3_ Third base
/// \tparam Base4_ Fourth base
/// \tparam Base5_ Fifth base
template <class Base1_ = void, class Base2_ = void, class Base3_ = void,
          class Base4_ = void, class Base5_ = void>
struct ManagedNode;

/// Node that does not inherit data.
template<> struct ManagedNode<void, void, void, void> {
    using Base1 = void; ///< First base
    REFLECTABLE()
};

/// Note that inherits data from a single base class.
/// \tparam Base1_ First base
template <class Base1_>
struct ManagedNode<Base1_, void, void, void> : public Base1_ {
    using Base1 = Base1_; ///< First base
    using Base2 = void;   ///< No second base
    REFLECTABLE()
};

/// Note that inherits data from two base classes.
/// \tparam Base1_ First base
/// \tparam Base2_ Second base
template <class Base1_, class Base2_>
struct ManagedNode<Base1_, Base2_, void, void>
    : public Base1_, public Base2_ {
    using Base1 = Base1_; ///< First base
    using Base2 = Base2_; ///< Second base
    using Base3 = void;   ///< No third base
    REFLECTABLE()
};

/// Note that inherits data from three base classes.
/// \tparam Base1_ First base
/// \tparam Base2_ Second base
/// \tparam Base3_ Third base
template <class Base1_, class Base2_, class Base3_>
struct ManagedNode<Base1_, Base2_, Base3_, void>
    : public Base1_, public Base2_, public Base3_ {
    using Base1 = Base1_; ///< First base
    using Base2 = Base2_; ///< Second base
    using Base3 = Base3_; ///< Third base
    using Base4 = void;   ///< No fourth base
    REFLECTABLE()
};

/// Note that inherits data from four base classes.
/// \tparam Base1_ First base
/// \tparam Base2_ Second base
/// \tparam Base3_ Third base
/// \tparam Base4_ Fourth base
template <class Base1_, class Base2_, class Base3_, class Base4_>
struct ManagedNode<Base1_, Base2_, Base3_, Base4_>
    : public Base1_, public Base2_, public Base3_, public Base4_ {
    using Base1 = Base1_; ///< First base
    using Base2 = Base2_; ///< Second base
    using Base3 = Base3_; ///< Third base
    using Base4 = Base4_; ///< Fourth base
    using Base5 = void;   ///< No fifth base
    REFLECTABLE()
};

}
 
#endif
