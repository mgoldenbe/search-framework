#ifndef VISUAL_LOG
#define VISUAL_LOG

#include <map>
#include <unordered_map>

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
            vertexStyles_[vd] = VisualEvent::defaultVertexStyle();
        for (auto ed : g_.edgeRange())
            edgeStyles_[ed] = VisualEvent::defaultEdgeStyle();

        step_ = 0;
        for (auto &e: log.events()) {
            filteredToStep_.push_back(step_);
            stepToFiltered_.push_back(step_);
            auto ve = VisualEvent(g_, e, (*this));
            this->log(ve);
        }
        // rewind to the beginning
        while (step_ > 0)
            prev();
    }

    const VertexStyle &vertexStyle(VertexDescriptor vd) const {
        auto it = vertexStyles_.find(vd);
        if (it == vertexStyles_.end()) assert(0);
        return it->second;
    }
    const EdgeStyle &edgeStyle(EdgeDescriptor ed) const {
        auto it = edgeStyles_.find(ed);
        if (it == edgeStyles_.end()) assert(0);
        return it->second;
    }

    bool inFilter(int step) const {
        return stepToFiltered_[step] != -1;
    }

    int nEvents() const { return events_.size(); }

    int nFilteredEvents() const { return filteredToStep_.size(); }

    int stepToFiltered(int step) const { return stepToFiltered_[step]; }

    int filteredToStep(int step) const { return filteredToStep_[step]; }

    void move(int step) {
        while (step_ != step) {
            step_ < step ? next() : prev();
        }
    }

    bool next() {
        if (step_ - 1 == filteredToStep_.back()) return false;
        do {
            if (!stepForward()) return false;
        } while (!inFilter(step_ - 1));
        return true;
    }

    bool prev() {
        do {
            if (!stepBackward()) return false;
        } while (!inFilter(step_ - 1));
        return true;
    }

    template <class Filter> bool next(const Filter &searchFilter) {
        int origStep = step_;
        do {
            if (!next()) {move(origStep); return false;}
        } while (!searchFilter.in(algorithmEvent(step_ - 1)));
        return true;
    }

    template <class Filter> bool prev(const Filter &searchFilter) {
        int origStep = step_;
        do {
            if (!prev()) {move(origStep); return false;}
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

    template <class Filter> void setFilter(const Filter &filter) {
        filteredToStep_.clear();
        for (auto &e : log_.events())
            if (filter.in(e)) filteredToStep_.push_back(e.step());

        stepToFiltered_.resize(events_.size());
        std::fill(stepToFiltered_.begin(), stepToFiltered_.end(), -1);
        int filteredStep = 0;
        for (auto step : filteredToStep_)
            stepToFiltered_[step] = filteredStep++;
        if (!inFilter(step_)) prev();
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

    bool stepForward() {
        if (step_ >= events_.size()) return false;
        auto e = events_[step_++];
        applyEvent(e);
        return true;
    }

    bool stepBackward() {
        if (step_ <= 0) return false;
        auto e = events_[--step_];
        unApplyEvent(e);
        return true;
    }

    void applyEvent(const VisualEvent &e) {
        for (auto &vertexChange: e.vertexChanges())
            vertexStyles_[vertexChange.vd] = vertexChange.now;
        for (auto &edgeChange : e.edgeChanges())
            edgeStyles_[edgeChange.ed] = edgeChange.now;
    }

    void unApplyEvent(const VisualEvent &e) {
        for (auto &vertexChange: e.vertexChanges())
            vertexStyles_[vertexChange.vd] = vertexChange.before;
        for (auto &edgeChange : e.edgeChanges())
            edgeStyles_[edgeChange.ed] = edgeChange.before;
    }

    void log(const VisualEvent &e) {
        applyEvent(e);
        events_.push_back(e);
        step_++;
    }
};

#endif
