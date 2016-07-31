///@file
///@brief INTERFACES CHECKED.

#ifndef UTIL_HASH_H
#define UTIL_HASH_H

template <class State>
struct StateHash {
    std::size_t operator()(const State &s) const {
        return s.hash();
    }
};

template <class State>
struct StateSharedPtrHash {
    std::size_t operator()(const deref_shared_ptr<const State> &s) const {
        return s->hash();
    }
};

#endif
