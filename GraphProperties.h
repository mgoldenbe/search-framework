#ifndef GRAPH_PROPERTIES
#define GRAPH_PROPERTIES

template <class Graph>
struct Logger {
    using VertexDescriptor = Graph::VertexDescriptor;
    enum class VStatus { INVISIBLE, VISIBLE, OPEN, CLOSED, SELECTED };
    enum class VRole { REGULAR, START, GOAL };
    using VParent = VertexDescriptor;
    using VInfo = struct {
        std::string str_;
    };
    using VChange = struct {
        std::string str_;
    };
    using VProperties = struct {
        VStatus status;
        VRole role;
        VParent parent;
        VInfo info;
        VChange change;
        VProperties(bool visibleFlag) {
            status = visibleFlag ? VStatus::VISIBLE : VStatus::INVISIBLE;
            role = REGULAR;
            parent = nullptr;
            info = vp.change = "";
        }
    };
    using LogEntry = struct {
        VertexDescriptor vd;
        VertexDescriptor selected;
        VProperties properties;
        LogEntry(bool visibleFlag)
            : properties(visibleFlag) {
            vd = nullptr;
            selected = nullptr;
        }
    };

    Logger(Graph g, bool newVisibleFlag)
        : g_(g), newVisibleFlag_(newVisibleFlag) {
        // insert a dummy first entry
        log.push_back(LogEntry(newVisibleFlag_));
        cur_ = 0;
    }

    void addVertex(VertexDescriptor vd) {
        propertiesMap[vd] = VProperties(vd, newVisibleFlag_);
    }

    void setRole(VertexDescriptor vd, VRole role) {
        propertiesMap[vd].role = role;
    }

    void log(VertexDescriptor vd, VStatus status, VChange change = "") {
        LogEntry entry(log_[cur_]);
        entry.vd = vd;
        if (status == VStatus::CLOSED) selected = nullptr;
        if (status == VStatus::OPEN) selected = vd;

        VProperties &properties = entry.properties;
        properties.status = status;
        if (change != "") {
            assert(status = VStatus::OPEN);
            properties.change = change;
        }
        log.push_back(entry);
    }

    VStatus status(VertexDescriptor vd, VStatus status) {
        auto res = status(vd);
        p_[vd].status = status;
        return res;
    }

    VRole role(VertexDescriptor vd) {return p_[vd].role;}
    VRole role(VertexDescriptor vd, VRole role) {
        auto res = role(vd);
        p_[vd].role = role;
        return res;
    }

    VParent parent(VertexDescriptor vd) {return p_[vd].parent;}
    VParent parent(VertexDescriptor vd, VParent parent) {
        auto res = parent(vd);
        p_[vd].parent = parent;
        return res;
    }

    VInfo info(VertexDescriptor vd) {return p_[vd].info;}
    VInfo info(VertexDescriptor vd, VInfo info) {
        auto res = info(vd);
        p_[vd].info = info;
        return res;
    }

    VChange change(VertexDescriptor vd) {return p_[vd].change;}
    VChange change(VertexDescriptor vd, VChange change) {
        auto res = change(vd);
        p_[vd].change = change;
        return res;
    }

private:
    const Graph &g_;
    std::map<VertexDescriptor, VProperties> propertiesMap_;
    std::vector<LogEntry> log_;
    int curStep_ = -1;
};

template <class Graph>
struct GraphLog {
    using VertexProperties = GraphProperties::VertexProperties;
    void add(VertexProperties vp) {
        log_.push_back(vp);
        if (curIndex_ < 0) {
            curIndex_ = 0;
            cur_ = vp;
        }
    }

    bool next() {
        if (curIndex_ >= log_.size() - 1) return false;
        curIndex_++;
        cur_ = log_[curIndex];
        return true;
    }

    bool prev() {
        if (curIndex_ < 1) return false;
        curIndex_--;
        cur_ = log_[curIndex];
        return true;
    }

    VertexProperties current() {
        assert(curIndex_ >= 0 && curIndex_ < log_.size());
        return cur_;
    }


}

#endif
