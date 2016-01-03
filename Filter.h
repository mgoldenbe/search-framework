#ifndef FILTER_H
#define FILTER_H

#include <memory>

template <class AlgorithmEvent>
struct FilterBase {
    virtual bool in(const AlgorithmEvent &e) const = 0;
};

template <class AlgorithmEvent>
struct FilterEventType: FilterBase<AlgorithmEvent> {
    FilterEventType() {set(AlgorithmEvent::eventTypeStr);}
    virtual bool in(const AlgorithmEvent &e) const {
        std::string type =
            AlgorithmEvent::eventTypeStr[static_cast<int>(e.type())];
        return std::find(types_.begin(), types_.end(), type) != types_.end();
    }
    void set(const std::vector<std::string> &types) { types_ = types; }
    const std::vector<std::string> &get() const { return types_; }

private:
    std::vector<std::string> types_;
};

template <class AlgorithmEvent>
struct Filter: FilterBase<AlgorithmEvent> {
    Filter() : filters_({&filterEventType_}) {}
    FilterEventType<AlgorithmEvent> &filterEventType() {
        return filterEventType_;
    }
    virtual bool in(const AlgorithmEvent &e) const override {
        for (auto f : filters_)
            if (!f->in(e)) return false;
        return true;
    }
private:
    FilterEventType<AlgorithmEvent> filterEventType_;
    std::vector<FilterBase<AlgorithmEvent> *> filters_;
};

#endif
