#ifndef EVENT_TYPES_H
#define EVENTS_TYPES_H

namespace Events {

/// No visual change. Used only for eventStr() and for the message enum;
template <class Node = SLB_NODE> struct NoChange : Base<Node> {
    using Events::Base<Node>::Base;
};

template <class Node = SLB_NODE> struct UniformChange : Base<Node> {
    using State = typename Node::State;
    using StateSharedPtr = std::shared_ptr<const State>;
    using StateSet = std::vector<StateSharedPtr>;
    using Edge = std::pair<StateSharedPtr, StateSharedPtr>;
    using EdgeSet = std::vector<Edge>;
    using Events::Base<Node>::Base;
    using VisualChanges = typename Events::Base<Node>::VisualChanges;

    VisualChanges
    visualChanges(const CurrentStyles<State> &styles) const {
        VisualChanges res;
        StateSet states;
        EdgeSet edges;
        sets(states, edges);
        for (auto &s : states) {
            VertexStyle before = styles.get(s);
            VertexStyle now = before;
            change(now, s);
            res.vChanges.push_back({s, now, before});
        }
        for (auto &a : edges) {
            EdgeStyle before = styles.get(a.first, a.second);
            EdgeStyle now = before;
            change(now, a);
            res.eChanges.push_back({a.first, a.second, now, before});
        }
        return res;
    }

protected:
    virtual void sets(StateSet &, EdgeSet &) const = 0;
    virtual void change(VertexStyle &, const StateSharedPtr &) const {};
    virtual void change(EdgeStyle &, const Edge &) const {};
};

template <class Node = SLB_NODE> struct VertexChange : UniformChange<Node> {
    using DirectBase = UniformChange<Node>;
    using StateSet = typename DirectBase::StateSet;
    using EdgeSet = typename DirectBase::EdgeSet;

    using UniformChange<Node>::UniformChange;
    using Base<Node>::state_;

protected:
    virtual void sets(StateSet &states, EdgeSet &) const {
        states.push_back(state_);
    }
};

template <class Node = SLB_NODE> struct VertexEdgeChange : UniformChange<Node> {
    using DirectBase = UniformChange<Node>;
    using StateSet = typename DirectBase::StateSet;
    using EdgeSet = typename DirectBase::EdgeSet;

    using UniformChange<Node>::UniformChange;
    using Base<Node>::state_;
    using Base<Node>::parent_;
    using Base<Node>::parentSubstitution_;

protected:
    virtual void sets(StateSet &states, EdgeSet &edges) const {
        states.push_back(state_);
        if (parentSubstitution_ != nullptr)
            edges.push_back({parentSubstitution_, state_});
        else if (parent_ != nullptr)
            edges.push_back({parent_, state_});
    }
};

template <class Node = SLB_NODE> struct PathChange : UniformChange<Node> {
    using DirectBase = UniformChange<Node>;
    using StateSet = typename DirectBase::StateSet;
    using EdgeSet = typename DirectBase::EdgeSet;

    using UniformChange<Node>::UniformChange;

protected:
    virtual void sets(StateSet &states, EdgeSet &edges) const {
        states = path();
        for (int i = 0; i != states.size() - 1; ++i)
            edges.push_back({states[i], states[i + 1]});
    }
    virtual StateSet path() const = 0;
};

template <class Node = SLB_NODE> struct SolutionPathChange : PathChange<Node> {
    using DirectBase = PathChange<Node>;
    using StateSet = typename DirectBase::StateSet;
    using PathChange<Node>::PathChange;
    using Base<Node>::state_;

protected:
    virtual StateSet path() const {
        return Base<Node>::path(state_);
    };
};

template <class Node = SLB_NODE> struct VertexEmphasis : VertexChange<Node> {
    using StateSharedPtr = typename Base<Node>::StateSharedPtr;
    using VertexChange<Node>::VertexChange;

protected:
    virtual void change(VertexStyle &style,
                        const StateSharedPtr &) const override {
        style.emphasisColor = color();
        style.emphasisSizeFactor = size();
    };
private:
    virtual Color color() const = 0;
    virtual double size() const = 0;
};

template <class Node = SLB_NODE> struct VertexColor : VertexChange<Node> {
    using StateSharedPtr = typename Base<Node>::StateSharedPtr;
    using VertexChange<Node>::VertexChange;

protected:
    virtual void change(VertexStyle &style,
                        const StateSharedPtr &) const override {
        style.fillColor = color();
    };

    virtual Color color() const = 0;
};

template <class Node = SLB_NODE> struct VertexEdgeColor : VertexEdgeChange<Node> {
    using StateSharedPtr = typename UniformChange<Node>::StateSharedPtr;
    using Edge = typename UniformChange<Node>::Edge;
    using VertexEdgeChange<Node>::VertexEdgeChange;

protected:
    virtual void change(VertexStyle &style,
                        const StateSharedPtr &) const override {
        style.fillColor = color();
    };

    virtual void change(EdgeStyle &style, const Edge &) const override {
        style.color = color();
    };

    virtual Color color() const = 0;
};
}
#endif
