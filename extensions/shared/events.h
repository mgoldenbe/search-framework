#ifndef EVENTS_H
#define EVENTS_H

namespace Events {

/// Previous event should be for the same state.
template <class Node = SLB_NODE> struct HideLast : NoChange<Node> {
    using NoChange<Node>::NoChange;

private:
    std::string eventStr() const override { return "Hide Last Event"; }
    EventType eventType() const { return EventType::HIDE_LAST_EVENT; }
};

template <class Node = SLB_NODE> struct MarkedStart : VertexEmphasis<Node> {
    using VertexEmphasis<Node>::VertexEmphasis;

private:
    virtual ::Color color() const override { return ::Color::VIVID_GREEN; }
    virtual double size() const override { return 0.5; }
    std::string eventStr() const override { return "Marked Start"; }
};

template <class Node = SLB_NODE> struct MarkedGoal : VertexEmphasis<Node> {
    using VertexEmphasis<Node>::VertexEmphasis;

private:
    virtual ::Color color() const override { return ::Color::DEEP_BLUE; }
    virtual double size() const override { return 0.5; }
    std::string eventStr() const override { return "Marked Goal"; }
};

template <class Node = SLB_NODE> struct Generated : VertexEdgeColor<Node> {
    using VertexEdgeColor<Node>::VertexEdgeColor;

private:
    virtual ::Color color() const override { return ::Color::SUNSHINE_YELLOW; }
    std::string eventStr() const override { return "Generated"; }
};

template <class Node = SLB_NODE> struct NotGenerated : VertexEdgeColor<Node> {
    using VertexEdgeColor<Node>::VertexEdgeColor;

private:
    virtual ::Color color() const override { return ::Color::WHITE; }
    std::string eventStr() const override { return "Not Generated"; }
};

template <class Node = SLB_NODE> struct EnteredOpen : VertexEdgeColor<Node> {
    using VertexEdgeColor<Node>::VertexEdgeColor;

private:
    virtual ::Color color() const override { return ::Color::PALE_YELLOW; }
    std::string eventStr() const override { return "Entered OPEN"; }
};

template <class Node = SLB_NODE> struct Selected : VertexColor<Node> {
    using VertexColor<Node>::VertexColor;
    static ::Color color_;

private:
    virtual ::Color color() const override { return color_; }
    std::string eventStr() const override { return "Selected"; }
};
template <class Node>::Color Selected<Node>::color_ = ::Color::VIVID_PURPLE;

template <class Node = SLB_NODE> struct SuspendedExpansion : VertexColor<Node> {
    using VertexColor<Node>::VertexColor;

private:
    virtual ::Color color() const override { return ::Color::ORANGE; }
    std::string eventStr() const override { return "Suspended Expansion"; }
};

template <class Node = SLB_NODE> struct ResumedExpansion : VertexColor<Node> {
    using VertexColor<Node>::VertexColor;

private:
    virtual ::Color color() const override { return ::Color::RICH_PURPLE; }
    std::string eventStr() const override { return "Resumed Expansion"; }
};

template <class Node = SLB_NODE> struct DeniedExpansion : VertexColor<Node> {
    using VertexColor<Node>::VertexColor;

private:
    virtual ::Color color() const override { return ::Color::YELLOWISH_TAN; }
    std::string eventStr() const override { return "Denied Expansion"; }
};

template <class Node = SLB_NODE> struct Closed : VertexEdgeColor<Node> {
    using VertexEdgeColor<Node>::VertexEdgeColor;
    static ::Color color_;

private:
    virtual ::Color color() const override { return color_; }
    std::string eventStr() const override { return "Closed"; }
};
template <class Node>::Color Closed<Node>::color_ = ::Color::WARM_BROWN;

template <class Node = SLB_NODE> struct SolvedGoal : SolutionPathChange<Node> {
    using StateSharedPtr = typename UniformChange<Node>::StateSharedPtr;
    using Arc = typename UniformChange<Node>::Arc;
    using Base<Node>::state_;
    using SolutionPathChange<Node>::SolutionPathChange;

protected:
    virtual void change(EdgeStyle &style, const Arc &) const override {
        style.color = color_;
        style.arrow = true;
    };

private:
    ::Color color_ = ::Color::VIVID_GREEN;
    std::string eventStr() const override { return "Solved Goal"; }
};

}
#endif
