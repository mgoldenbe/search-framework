#ifndef EVENT_TYPES_H
#define EVENTS_TYPES_H

namespace Events {

template <class Node = SLB_NODE> struct UniformChange : Base<Node> {
    using State = typename Node::State;
    using StateSharedPtr = std::shared_ptr<const State>;
    using StateSet = std::vector<StateSharedPtr>;
    using Arc = std::pair<StateSharedPtr, StateSharedPtr>;
    using ArcSet = std::vector<Arc>;
    using Events::Base<Node>::Base;
    using VisualChanges = typename Events::Base<Node>::VisualChanges;

    VisualChanges
    visualChanges(const CurrentStyles<State> &styles) const {
        VisualChanges res;
        StateSet states;
        ArcSet arcs;
        sets(states, arcs);
        for (auto &s : states) {
            VertexStyle style = styles.get(s);
            change(style, s);
            res.vChanges.push_back({s, style});
        }
        for (auto &a : arcs) {
            EdgeStyle style = styles.get(a.first, a.second);
            change(style, a);
            res.aChanges.push_back({a.first, a.second, style});
        }
        return res;
    }

protected:
    virtual void sets(StateSet &, ArcSet &) const = 0;
    virtual void change(VertexStyle &, const StateSharedPtr &) const {};
    virtual void change(EdgeStyle &, const Arc &) const {};
};

template <class Node = SLB_NODE> struct VertexChange : UniformChange<Node> {
    using DirectBase = UniformChange<Node>;
    using StateSet = typename DirectBase::StateSet;
    using ArcSet = typename DirectBase::ArcSet;

    using UniformChange<Node>::UniformChange;
    using Base<Node>::state_;

protected:
    virtual void sets(StateSet &states, ArcSet &) const {
        states.push_back(state_);
    }
};

template <class Node = SLB_NODE> struct VertexEdgeChange : UniformChange<Node> {
    using DirectBase = UniformChange<Node>;
    using StateSet = typename DirectBase::StateSet;
    using ArcSet = typename DirectBase::ArcSet;

    using UniformChange<Node>::UniformChange;
    using Base<Node>::state_;
    using Base<Node>::parent_;

protected:
    virtual void sets(StateSet &states, ArcSet &arcs) const {
        states.push_back(state_);
        if (parent_ != nullptr) arcs.push_back({parent_, state_});
    }
};

template <class Node = SLB_NODE> struct PathChange : UniformChange<Node> {
    using DirectBase = UniformChange<Node>;
    using StateSet = typename DirectBase::StateSet;
    using ArcSet = typename DirectBase::ArcSet;

    using UniformChange<Node>::UniformChange;

protected:
    virtual void sets(StateSet &states, ArcSet &arcs) const {
        states = path();
        for (int i = 0; i != states.size() - 1; ++i)
            arcs.push_back({states[i], states[i + 1]});
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
    using Arc = typename UniformChange<Node>::Arc;
    using VertexEdgeChange<Node>::VertexEdgeChange;

protected:
    virtual void change(VertexStyle &style,
                        const StateSharedPtr &) const override {
        style.fillColor = color();
    };

    virtual void change(EdgeStyle &style, const Arc &) const override {
        style.color = color();
    };

    virtual Color color() const = 0;
};
}
#endif
