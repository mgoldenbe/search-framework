#ifndef VECTOR_OF_PAIRS
#define VECTOR_OF_PAIRS

#include <vector>

template <typename T1, typename T2>
T1 findFirst(std::vector<std::pair<T1, T2>> &v, const T2 &el) {
    return std::find_if(v.begin(), v.end(), [el](std::pair<T1, T2> &p) {
                            return p.second == el;
                        })->first;
}

template <typename T1, typename T2>
T2 findSecond(std::vector<std::pair<T1, T2>> &v, const T1 &el) {
    return std::find_if(v.begin(), v.end(), [el](std::pair<T1, T2> &p) {
            return p.first == el;
        })->second;
}

template <typename T1, typename T2>
std::vector<T1> projectFirst(const std::vector<std::pair<T1, T2>> &v) {
    std::vector<T1> res;
    std::transform(v.begin(), v.end(), std::back_inserter(res),
                   [](const std::pair<T1, T2> &p) { return p.first; });
    return res;
}

template <typename T1, typename T2>
std::vector<T2> projectSecond(const std::vector<std::pair<T1, T2>> &v) {
    std::vector<T2> res;
    std::transform(v.begin(), v.end(), std::back_inserter(res),
                   [](const std::pair<T1, T2> &p) { return p.second; });
    return res;
}

#endif
