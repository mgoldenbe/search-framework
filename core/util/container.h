#ifndef SLB_CORE_UTIL_CONTAINER_H
#define SLB_CORE_UTIL_CONTAINER_H

///@file
///@brief Container utilities.
/// \author Meir Goldenberg

namespace slb {
namespace core {
namespace util {

/// Checks whether an element is contained in the container.
/// \tparam V The container type.
/// \tparam T The element type.
/// \param v The container.
/// \param el The element.
/// \return \c true if the element is contained in the container and \c false
/// otherwise.
template<class V, typename T>
bool in(const V &v, const T &el) {
    return std::find(v.begin(), v.end(), el) != v.end();
}

/// Checks whether an element is contained in the container. If the element is
/// contained, it is erased.
/// \tparam V The container type.
/// \tparam T The element type.
/// \param v The container.
/// \param el The element.
/// \return \c true if the element is (i.e. was) in the container and \c false
/// otherwise.
template<class V, typename T>
bool inErase(V &v, const T &el) {
    auto it = std::find(v.begin(), v.end(), el);
    if (it == v.end()) return false;
    v.erase(it);
    return true;
}

/// Push back a unique element.
/// \tparam V The container type.
/// \tparam T The element type.
/// \param v The container.
/// \param el The element.
/// \return Returns true if the element has been added now.
template<class V, typename T>
bool push_back_unique(V &v, const T &el) {
    if (!in(v, el)) {
        v.push_back(el);
        return true;
    }
    return false;
}

/// Shuffle a vector of std::reference_wrapper. The algorithm is version 1 from
/// http://en.cppreference.com/w/cpp/algorithm/random_shuffle
/// \tparam RandomIt The random iterator type.
/// \param first The beginning of the range.
/// \param last The end of the range.
template< class RandomIt >
void shuffleRefs( RandomIt first, RandomIt last ) {
    typename std::iterator_traits<RandomIt>::difference_type i, n;
    n = last - first;
    for (i = n-1; i > 0; --i) {
        using std::swap;
        swap(first[i].get(), first[std::rand() % (i+1)].get());
    }
}

/// Shuffle a map.
/// \tparam MapType The map type.
/// \param map The map.
template <class MapType>
void shuffleMap(MapType &map) {
    std::vector<std::reference_wrapper<typename MapType::mapped_type>> v;
    for (auto &el : map) v.push_back(std::ref(el.second));
    shuffleRefs(v.begin(), v.end());
}

/// Get an element with maximal value in a map.
/// \tparam MapType The map type.
/// \param x The map.
/// \return The element with maximal value in \c x.
template <class MapType>
typename MapType::value_type mapMax(const MapType &x) {
    using pairtype = typename MapType::value_type;
    return *std::max_element(x.begin(), x.end(),
                             [](const pairtype &p1, const pairtype &p2) {
                                 return p1.second < p2.second;
                             });
}

/// Get the vector of map's keys.
/// \tparam MapType The map type.
/// \param x The map.
/// \return The vector of the keys in \c x.
template <class MapType>
std::vector<typename MapType::key_type> mapKeys(const MapType &x) {
    std::vector<typename MapType::key_type> res;
    for (const auto &el: x) res.push_back(el.first);
    return res;
}

/// Adapter for iterator that knows whether it is at the end.
/// \tparam Base The base iterator, can be Constainer::iterator or
/// Container::const_iterator.
template <class Container, class Base>
struct BasicInformedIterator: Base {
    BasicInformedIterator(Container &c): Base(), c_(c) {}
    BasicInformedIterator(Container &c, Base b): Base(b), c_(c) {}
    BasicInformedIterator(const BasicInformedIterator &b) : Base(b), c_(b.c_) {}
    bool isBegin() const {return *this == c_.begin();}
    bool isEnd() const {return *this == c_.end();}
    Container &container() const { return c_; }

    BasicInformedIterator &operator=(const BasicInformedIterator &rhs) {
        Base::operator=(rhs);
        assert(&c_ == &rhs.c_);
        return *this;
    }

    int index() const { return *this - c_.begin(); }

private:
    Container &c_;
};

/// Skipping iterators for a vector. Use the aliases defined below.
template <class MyVector, class BaseIterator>
struct BasicVectorSkipIterator : BasicInformedIterator<MyVector, BaseIterator> {
    using Base = BasicInformedIterator<MyVector, BaseIterator>;
    using ValueType = typename MyVector::value_type;

    BasicVectorSkipIterator(MyVector &v, BaseIterator it, const ValueType &skip)
        : Base(v, it), skip_(skip) {
        adjust();
    }

    BasicVectorSkipIterator(const BasicVectorSkipIterator &b)
        : Base(b), skip_(b.skip_) {}

    BasicVectorSkipIterator &operator++() {
        Base::operator++();
        adjust();
        return *this;
    }

    BasicVectorSkipIterator &operator=(const BasicVectorSkipIterator &rhs) {
        Base::operator=(rhs);
        skip_ = rhs.skip_;
        return *this;
    }

private:
    ValueType skip_;

    void adjust() {
        while (!this->isEnd() && Base::operator*() == skip_) Base::operator++();
    }
};

template <class MyVector>
using VectorSkipIterator =
    BasicVectorSkipIterator<MyVector, typename MyVector::iterator>;

template <class MyVector>
using VectorSkipConstIterator =
    BasicVectorSkipIterator<typename std::add_const<MyVector>::type,
                            typename MyVector::const_iterator>;

// Vector iterator whose operator* returns index instead of value.
template <class MyVector, class Base>
struct IndexIterator : Base {
    template <class... Args>
    IndexIterator(Args &&... args) : Base(args...) {}

    IndexIterator(const IndexIterator &b) : Base(b) {}

    int operator*() const {
        return *this - this->container().begin();
    }

    IndexIterator &operator=(const IndexIterator &rhs) {
        Base::operator=(rhs);
        return *this;
    }
};

/* Need to re-design this if ever have a need for it.
/// Iterate on a vector based on the order provided by values returned by
/// de-referencing the Base iterator (which may refer to another container).
template <class MyVector, class Base, class Translator>
struct IndirectIterator: Base {
    using ValueType = typename MyVector::value_type;

    template <class... Args>
    IndirectIterator(MyVector &v, const Translator &t, Args &&... args)
        : Base(args...), v_(v), t_(t) {}

    IndirectIterator(MyVector &v, const Translator &t, const Base &b)
        : Base(b), v_(v), t_(t) {}

    ValueType operator*() { return v_[t_(Base::operator*())]; }

    IndirectIterator &operator=(const IndirectIterator &rhs) {
        Base::operator=(rhs);
        v_ = rhs.v_;
        t_ = rhs.t_;
        return *this;
    };
private:
    MyVector &v_;
    Translator t_;
};
*/

// The following iterators are based on:
// http://stackoverflow.com/a/35262398/2725810

/// Iterator over keys of a map.
template<class MyMap>
struct MapKeyIterator : MyMap::iterator {
    using Base = typename MyMap::iterator;
    using Key = typename MyMap::key_type;
    MapKeyIterator() : Base(){};
    MapKeyIterator(Base it_) : Base(it_){};

    Key *operator->() const { return &(Base::operator->()->first); }
    Key operator*() const { return Base::operator*().first; }
};

/// Const iterator over keys of a map.
template<class MyMap>
struct MapKeyConstIterator : MyMap::const_iterator {
    using Base = typename MyMap::const_iterator;
    using Key = typename MyMap::key_type;
    MapKeyConstIterator() : Base(){};
    MapKeyConstIterator(Base it_) : Base(it_){};

    Key *operator->() const { return &(Base::operator->()->first); }
    Key operator*() const { return Base::operator*().first; }
};

/// Iterator over values of a map.
template<class MyMap>
struct MapValIterator : MyMap::iterator {
    using Base = typename MyMap::iterator;
    using Val = typename MyMap::mapped_type;
    MapValIterator() : Base(){};
    MapValIterator(Base it_) : Base(it_){};

    Val *operator->() const { return &(Base::operator->()->second); }
    Val operator*() const { return Base::operator*().second; }
};

/// Const iterator over values of a map.
template<class MyMap>
struct MapValConstIterator : MyMap::const_iterator {
    using Base = typename MyMap::const_iterator;
    using Val = typename MyMap::mapped_type;
    MapValConstIterator() : Base(){};
    MapValConstIterator(Base it_) : Base(it_){};

    const Val *operator->() const { return &(Base::operator->()->second); }
    Val operator*() const { return Base::operator*().second; }
};

} // namespace
} // namespace
} // namespace

#endif
