#ifndef VISUAL_LOG
#define VISUAL_LOG

#include <map>
#include <unordered_map>
#include "VisualizationUtilities.h"

template <class VisualEvent>
struct NoVisualLog {
    using Graph = typename VisualEvent::Graph;
    using VertexDescriptor = typename Graph::VertexDescriptor;
    using EdgeDescriptor = typename Graph::EdgeDescriptor;

    void log(VisualEvent e) {
        (void)e;
    }
};

template <class AlgorithmLog_, class VisualEvent_>
struct VisualLog {
    using AlgorithmLog = AlgorithmLog_;
    using AlgorithmEvent = typename AlgorithmLog::AlgorithmEvent;
    using State= typename AlgorithmEvent::State;
    using VisualEvent = VisualEvent_;
    using Graph = typename VisualEvent::Graph;
    using VertexDescriptor = typename Graph::VertexDescriptor;
    using EdgeDescriptor = typename Graph::EdgeDescriptor;
    using VertexStyle = typename VisualEvent::VertexStyle;
    using EdgeStyle = typename VisualEvent::EdgeStyle;

    VisualLog(const AlgorithmLog &log, const Graph &g) : log_(log), g_(g) {
        for (auto vd : g_.vertexRange())
            vertexStyles_[vd] = VertexStyle();
        for (auto ed : g_.edgeRange())
            edgeStyles_[ed] = EdgeStyle();

        step_ = 0;
        for (auto &e: log.events()) {
            filteredToStep_.push_back(step_);
            stepToFiltered_.push_back(step_);
            auto ve = VisualEvent(g_, e, (*this));
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

    VisualEvent event(int step) const { return events_[step]; }

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

    const AlgorithmLog &algorithmLog() const { return log_; }

    const AlgorithmEvent &algorithmEvent(int step) const {
        return log_.events()[step];
    }

    template <class Filter, class Drawer>
    void setFilter(const Filter &filter, Drawer &drawer) {
        filteredToStep_.clear();
        for (auto &e : log_.events())
            if (filter.in(e)) filteredToStep_.push_back(e.step());

        stepToFiltered_.resize(events_.size());
        std::fill(stepToFiltered_.begin(), stepToFiltered_.end(), -1);
        int filteredStep = 0;
        for (auto step : filteredToStep_)
            stepToFiltered_[step] = filteredStep++;
        if (!inFilter(step_)) prev(drawer);
    }

private:
    const AlgorithmLog &log_;
    const Graph &g_;

    // -1 means no previous event
    std::unordered_map<VertexDescriptor, VertexStyle> vertexStyles_;

    // EdgeDescriptor cannot be a key for unordered_map
    std::map<EdgeDescriptor, EdgeStyle> edgeStyles_;

    std::vector<VisualEvent> events_;
    int step_; // the event to be applied when next() is called.
    std::vector<int> filteredToStep_;
    std::vector<int> stepToFiltered_; // -1 if not in filtered

    template <class Drawer> bool stepForward(Drawer &drawer) {
        if (step_ >= events_.size()) return false;
        auto e = events_[step_];
        applyEvent(e);
        step_++;
        e.draw(drawer);
        return true;
    }

    template <class Drawer> bool stepBackward(Drawer &drawer) {
        if (step_ <= 0) return false;
        auto e = events_[step_ - 1];
        unApplyEvent(e);
        step_--;
        e.draw(drawer, true);
        return true;
    }

    void applyEvent(const VisualEvent &e) {
        for (auto &vertexChange: e.vertexChanges())
            vertexStyles_[vertexChange.vd] = vertexChange.now;

        for (auto &edgeChange : e.edgeChanges()) {
            edgeStyles_[edgeChange.ed] = edgeChange.now;
            edgeStyles_[edgeChange.ed].depth = 2;
        }
        if (step_ > 0)
            for (auto &edgeChange : events_[step_ - 1].edgeChanges())
                edgeStyles_[edgeChange.ed].depth = 1;
    }

    void unApplyEvent(const VisualEvent &e) {
        for (auto &vertexChange: e.vertexChanges())
            vertexStyles_[vertexChange.vd] = vertexChange.before;

        for (auto &edgeChange : e.edgeChanges()) {
            edgeStyles_[edgeChange.ed] = edgeChange.before;
            edgeStyles_[edgeChange.ed].depth = edgeChange.before.depth;
        }
    }

    void log(const VisualEvent &e) {
        applyEvent(e);
        events_.push_back(e);
        step_++;
    }
};

#endif
