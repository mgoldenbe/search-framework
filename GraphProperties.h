#ifndef GRAPH_PROPERTIES
#define GRAPH_PROPERTIES

template <class Graph>
struct GraphProperties {
    using VertexDescriptor = Graph::VertexDescriptor;
    enum class VStatus { INVISIBLE, VISIBLE, OPEN, CLOSED, FOCUSED };
    enum class VRole { REGULAR, START, GOAL };
    using VParent = VertexDescriptor;
    using VInfo = std::string;
    using VChange = std::string;
    using VertexProperties = struct {
        VStatus status;
        VRole role;
        VParent parent;
        VInfo info;
        VChange change;
    };

    GraphProperties(const Graph &g, bool visibleFlag) {
        VertexProperties vp;
        vp.status = visibleFlag ? VStatus::VISIBLE : VStatus::INVISIBLE;
        vp.role = REGULAR;
        vp.parent = nullptr;
        vp.info = vp.change = "";
        for (auto vd: g.VertexRange()) p_[vd] = vp;
    }

    VStatus status(VertexDescriptor vd) {return p_[vd].status;}
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
    std::map<VertexDescriptor, VertexProperties> p_;
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

private:
    const Graph &g_;
    std::vector<VertexProperties> log_;
    VertexProperties cur_;
    int curIndex_ = -1;
}

#endif
