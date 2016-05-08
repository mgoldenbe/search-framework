#ifndef VISUAL_LOG_H
#define VISUAL_LOG_H

#include "graphics_object.h"
#include "visual_event.h"

template <class Node = SLB_NODE>
struct VisualLog: CurrentStyles<typename Node::State> {
    using AlgorithmEvent = typename Events::Base<Node>::Event;
    using State = typename Node::State;
    using MyVisualEvent = VisualEvent<Node>;
    using Graph = typename MyVisualEvent::Graph;
    using VertexDescriptor = typename Graph::VertexDescriptor;
    using EdgeDescriptor = typename Graph::EdgeDescriptor;
    using VertexStyle = typename MyVisualEvent::VertexStyle;
    using EdgeStyle = typename MyVisualEvent::EdgeStyle;

    using Base = CurrentStyles<typename Node::State>;
    using Base::g_;
    using Base::vertexStyles_;
    using Base::edgeStyles_;

    VisualLog(const AlgorithmLog<Node> &log, const Graph &g)
        : Base(g), log_(log) {
        for (auto vd : g_.vertexRange())
            vertexStyles_[vd] = VertexStyle();
        for (auto ed : g_.edgeRange())
            edgeStyles_[ed] = EdgeStyle();

        step_ = 0;
        for (auto &e: log.events()) {
            filteredToStep_.push_back(step_);
            stepToFiltered_.push_back(step_);
            auto ve = MyVisualEvent(g_, e, (*this));
            this->log(ve);
        }
        // rewind to the beginning
        reset();
        //while (step_ > 0)
        //    prev(temp);
    }

    VertexStyle &vertexStyle(VertexDescriptor vd) {
        auto it = vertexStyles_.find(vd);
        if (it == vertexStyles_.end()) assert(0);
        return it->second;
    }
    EdgeStyle &edgeStyle(EdgeDescriptor ed) {
        auto it = edgeStyles_.find(ed);
        if (it == edgeStyles_.end()) assert(0);
        return it->second;
    }

    MyVisualEvent event(int step) const { return events_[step]; }

    bool inFilter(int step) const {
        return stepToFiltered_[step] != -1;
    }

    int nEvents() const { return events_.size(); }

    int nFilteredEvents() const { return filteredToStep_.size(); }

    int stepToFiltered(int step) const { return stepToFiltered_[step]; }

    int filteredToStep(int step) const { return filteredToStep_[step]; }

    template <class Drawer>
    void move(int step, Drawer &drawer, bool groupFlag = false) {
        GroupLock lock{groupFlag, drawer.graphics()}; (void)lock;
        while (step_ != step) {
            step_ < step ? next(drawer, false) : prev(drawer, false);
        }
    }

    template <class Drawer> bool next(Drawer &drawer, bool groupFlag = true) {
        //for (int i = 0; i < 2; i++) {
        GroupLock lock{groupFlag, drawer.graphics()}; (void)lock;
        if (step_ - 1 == filteredToStep_.back()) return false;
        do {
            if (!stepForward(drawer)) return false;
        } while (!inFilter(step_ - 1));
        //}
        return true;
    }

    template <class Drawer> bool prev(Drawer &drawer, bool groupFlag = true) {
        GroupLock lock{groupFlag, drawer.graphics()}; (void)lock;
        do {
            if (!stepBackward(drawer)) return false;
        } while (!inFilter(step_ - 1));
        return true;
    }

    template <class Filter, class Drawer>
    bool next(const Filter &searchFilter, Drawer &drawer) {
        GroupLock lock{true, drawer.graphics()}; (void)lock;
        int origStep = step_;
        do {
            if (!next(drawer, false)) {move(origStep, drawer); return false;}
        } while (!searchFilter.in(algorithmEvent(step_ - 1)));
        return true;
    }

    template <class Filter, class Drawer>
    bool prev(const Filter &searchFilter, Drawer &drawer) {
        GroupLock lock{true, drawer.graphics()}; (void)lock;
        int origStep = step_;
        do {
            if (!prev(drawer, false)) {move(origStep, drawer); return false;}
        } while (!searchFilter.in(algorithmEvent(step_ - 1)));
        return true;
    }

    void reset() {
        while (step_ > 0) {
            auto e = events_[--step_];
            unApplyEvent(e);
        }
    }

    int step() const { return step_; }

    const AlgorithmLog<Node> &algorithmLog() const { return log_; }

    Base &currentStyles() { return *this; }

    const AlgorithmEvent &algorithmEvent(int step) const {
        return log_.events()[step];
    }

    template <class Filter, class Drawer>
    void setFilter(const Filter &filter, Drawer &drawer) {
        filteredToStep_.clear();
        for (auto &e : log_.events())
            if (filter.in(e)) filteredToStep_.push_back(e->step());

        stepToFiltered_.resize(events_.size());
        std::fill(stepToFiltered_.begin(), stepToFiltered_.end(), -1);
        int filteredStep = 0;
        for (auto step : filteredToStep_)
            stepToFiltered_[step] = filteredStep++;
        if (!inFilter(step_)) prev(drawer);
    }

    bool stepForward() {
        if (step_ >= events_.size()) return false;
        auto e = events_[step_];
        applyEvent(e);
        step_++;
        return true;
    }

    bool stepBackward() {
        if (step_ <= 0) return false;
        auto e = events_[step_ - 1];
        unApplyEvent(e);
        step_--;
        return true;
    }

private:
    const AlgorithmLog<Node> &log_;

    std::vector<MyVisualEvent> events_;
    int step_; // the event to be applied when next() is called.
    std::vector<int> filteredToStep_;
    std::vector<int> stepToFiltered_; // -1 if not in filtered

    template <class Drawer> bool stepForward(Drawer &drawer) {
        if (!stepForward()) return false;
        auto e = events_[step_ - 1];
        e.draw(drawer);
        return true;
    }

    template <class Drawer> bool stepBackward(Drawer &drawer) {
        if (!stepBackward()) return false;
        auto e = events_[step_];
        e.draw(drawer, true);
        return true;
    }

    void applyEvent(const MyVisualEvent &e) {
        for (auto &vertexChange: e.vertexChanges())
            vertexStyles_[vertexChange.vd] = vertexChange.now;

        for (auto &edgeChange : e.edgeChanges())
            edgeStyles_[edgeChange.ed] = edgeChange.now;
    }

    void unApplyEvent(const MyVisualEvent &e) {
        for (auto &vertexChange: e.vertexChanges())
            vertexStyles_[vertexChange.vd] = vertexChange.before;

        for (auto &edgeChange : e.edgeChanges())
            edgeStyles_[edgeChange.ed] = edgeChange.before;
    }

    void log(const MyVisualEvent &e) {
        applyEvent(e);
        events_.push_back(e);
        step_++;
    }
};

#endif
