#ifndef UTIL_MEMORY
#define UTIL_MEMORY

/// Releases moves a pointer from \c std::unique_ptr to \c std::shared_ptr
template <typename T>
std::shared_ptr<T> unique2shared(std::unique_ptr<T> &p) {
    return std::shared_ptr<T>(p.release());
}

#endif
