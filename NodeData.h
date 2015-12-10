///@file
///@brief INTERFACES CHECKED.

#ifndef NODE_DATA
#define NODE_DATA

template <typename State_>
struct NodeBase {
    using State = State_;
    using CostType = typename State::CostType;
    CostType g = CostType(0), f = CostType(0); // These are public for ease of
                                               // access. I don't see any reason
                                               // for making them private and
                                               // having getters and setters.
    template<typename charT>
    std::basic_ostream<charT> &dump(std::basic_ostream<charT> &o) const {
        o << " (" << "g: " << g << ", " << "f: " << f << ")";
        return o;
    }
};
template <typename State>
void dump(const NodeBase<State> &n) { dumpT(n); }

template <typename State>
struct NoNodeData: public NodeBase<State> {};
template <typename State>
void dump(const NoNodeData<State> &n) { dumpT(n); }


#endif
