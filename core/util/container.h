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
/// \tparam KeyType The key type.
/// \tparam ValueType The value type.
/// \param x The map.
/// \return The element with maximal value in \c x.
template <typename KeyType, typename ValueType>
std::pair<KeyType, ValueType> mapMax(const std::map<KeyType, ValueType> &x) {
    using pairtype = std::pair<KeyType, ValueType>;
    return *std::max_element(x.begin(), x.end(),
                             [](const pairtype &p1, const pairtype &p2) {
                                 return p1.second < p2.second;
                             });
}

/// Get the vector of map's keys.
/// \tparam KeyType The key type.
/// \tparam ValueType The value type.
/// \param x The map.
/// \return The vector of the keys in \c x.
template <typename KeyType, typename ValueType>
std::vector<KeyType> mapKeys(const std::map<KeyType, ValueType> &x) {
    std::vector<KeyType> res;
    for (const auto &el: x) res.push_back(el.first);
    return res;
}

} // namespace
} // namespace
} // namespace

#endif
