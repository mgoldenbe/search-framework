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

} // namespace
} // namespace
} // namespace

#endif
