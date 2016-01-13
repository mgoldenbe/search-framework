#include <iostream>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include "PrettyPrint.h"

// Shuffle a vector of std::reference_wrapper
// The algorithm is version 1 from
// http://en.cppreference.com/w/cpp/algorithm/random_shuffle
template< class RandomIt >
void shuffleRefs( RandomIt first, RandomIt last ) {
    typename std::iterator_traits<RandomIt>::difference_type i, n;
    n = last - first;
    for (i = n-1; i > 0; --i) {
        using std::swap;
        swap(first[i].get(), first[std::rand() % (i+1)].get());
    }
}

template <class MapType>
void shuffleMap(MapType &map) {
    std::vector<std::reference_wrapper<typename MapType::mapped_type>> v;
    for (auto &el : map) v.push_back(std::ref(el.second));
    shuffleRefs(v.begin(), v.end());
}

int main(void)
{
    std::map<int, int> myMap = {{1, 100}, {2, 200}, {3, 300}, {4, 400}};
    shuffleMap(myMap);
    std::cout << myMap << std::endl;
    return (0);
}
