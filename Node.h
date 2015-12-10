///@file
///@brief INTERFACES CHECKED.

#ifndef NODE
#define NODE

#include <vector>
#include "StateNeighbor.h"
#include "NodeData.h"

template<typename State>
using StateUniquePtrT = std::unique_ptr<const State>;
template<typename State>
using StateSharedPtrT = std::shared_ptr<const State>;
template<typename State>
//using DefaultStateSmartPtrT = StateUniquePtrT<State>;
using DefaultStateSmartPtrT = StateSharedPtrT<State>;

template <typename State_, class NodeData_ = NoNodeData<State_>,
          template <class> class StateSmartPtrT = DefaultStateSmartPtrT,
          typename BucketPosition = int>
struct AStarNode : public NodeData_ {
    using State = State_;
    using StateSmartPtr = StateSmartPtrT<State>;
    using NodeData = NodeData_;
    using MyType = AStarNode<State, NodeData_, StateSmartPtrT, BucketPosition>;
    using NodeUniquePtr = std::unique_ptr<MyType>;

    ///@name Construction and Assignment
    //@{
    AStarNode(const State &s) : state_(new State(s)) {}
    AStarNode(StateUniquePtrT<State> &s) : state_(std::move(s)) {}
    //@}

    ///@name Read-Only Services
    //@{
    const State &state() const { return *state_; }

    const MyType *parent() const { return parent_; }

    bool operator==(const MyType &rhs) const {
        return *(this->state()) == *(rhs.state());
    }

    const StateSmartPtr &shareState() const { return state_; }

    const StateSmartPtr &shareParentState() const {
        static StateSmartPtr nullResult = StateSmartPtr(nullptr);
        if (!parent_) return nullResult;
        return parent_->state_;
    }

    const std::vector<State> path() const {
        std::vector<State> res;
        MyType *cur = this;
        while (cur) {
            res.push_back(cur->state());
            cur = cur->parent_;
        }
        std::reverse(res.begin(), res.end());
        return res;
    }

    template<typename charT>
    std::basic_ostream<charT>& dump(std::basic_ostream<charT>& o) const {
        o << *state_ << (NodeData)*this;
        return o;
    }
    //@}

    ///@name Modification
    //@{
    void setParent(MyType *parent) { parent_ = parent; }
    void setBucketPosition(BucketPosition l) { bucketPosition_ = l; }
    //@}

private:
    StateSmartPtr state_;
    MyType *parent_ = nullptr;
    BucketPosition bucketPosition_;
};

#endif
