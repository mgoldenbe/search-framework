#ifndef UTIL_VECTOR_OF_PAIRS_H
#define UTIL_VECTOR_OF_PAIRS_H

///@file
///@brief Utilities for working with a vector of pairs.
/// \author Meir Goldenberg

/// Find the first coordinate corresponding to the second one in a vector of pairs.
/// \tparam T1 The type of the first coordinate.
/// \tparam T2 The type of the second coordinate.
/// \param v The vector of pairs.
/// \param el The second coordinate.
/// \return The sought first coordinate.
template <typename T1, typename T2>
T1 findFirst(std::vector<std::pair<T1, T2>> &v, const T2 &el) {
    return std::find_if(v.begin(), v.end(), [el](std::pair<T1, T2> &p) {
                            return p.second == el;
                        })->first;
}

/// Find the second coordinate corresponding to the first one in a vector of pairs.
/// \tparam T1 The type of the first coordinate.
/// \tparam T2 The type of the second coordinate.
/// \param v The vector of pairs.
/// \param el The first coordinate.
/// \return The sought second coordinate.
template <typename T1, typename T2>
T2 findSecond(std::vector<std::pair<T1, T2>> &v, const T1 &el) {
    return std::find_if(v.begin(), v.end(), [el](std::pair<T1, T2> &p) {
            return p.first == el;
        })->second;
}

/// Compute the projection onto the first coordinate of a vector of pairs.
/// \tparam T1 The type of the first coordinate.
/// \tparam T2 The type of the second coordinate.
/// \param v The vector of pairs.
/// \return The projection onto the first coordinate.
template <typename T1, typename T2>
std::vector<T1> projectFirst(const std::vector<std::pair<T1, T2>> &v) {
    std::vector<T1> res;
    std::transform(v.begin(), v.end(), std::back_inserter(res),
                   [](const std::pair<T1, T2> &p) { return p.first; });
    return res;
}

/// Compute the projection onto the second coordinate of a vector of pairs.
/// \tparam T1 The type of the first coordinate.
/// \tparam T2 The type of the second coordinate.
/// \param v The vector of pairs.
/// \return The projection onto the second coordinate.
template <typename T1, typename T2>
std::vector<T2> projectSecond(const std::vector<std::pair<T1, T2>> &v) {
    std::vector<T2> res;
    std::transform(v.begin(), v.end(), std::back_inserter(res),
                   [](const std::pair<T1, T2> &p) { return p.second; });
    return res;
}

#endif
