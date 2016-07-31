#ifndef UTIL_MEMORY
#define UTIL_MEMORY

/// Releases moves a pointer from \c std::unique_ptr to \c std::shared_ptr
template <typename T>
std::shared_ptr<T> unique2shared(std::unique_ptr<T> &p) {
    return std::shared_ptr<T>(p.release());
}

/// Shared pointer whose \c operator== compares the pointed to values.
template <typename T>
class deref_shared_ptr: private std::shared_ptr<T> {
public:
    using Base = std::shared_ptr<T>;

    using Base::operator->;
    using Base::operator*;
    using Base::operator bool;
    using Base::reset;

    constexpr deref_shared_ptr() : Base() {}
    constexpr deref_shared_ptr(std::nullptr_t) : Base(nullptr) {}
    deref_shared_ptr(const deref_shared_ptr &rhs) : Base(rhs) {}
    template <class Y> explicit deref_shared_ptr(Y *ptr) : Base(ptr) {}
    template <class Y, class Deleter>
    deref_shared_ptr(std::unique_ptr<Y, Deleter> &&r) : Base(std::move(r)) {}

    deref_shared_ptr &operator=(const deref_shared_ptr &rhs) {
        this->Base::operator=(rhs);
        return *this;
    }

    template <class TT, class... Args>
    friend deref_shared_ptr<TT> make_deref_shared(Args &&... args);

private:
    deref_shared_ptr(const Base &p) : Base(p) {}
};

template <class T, class U>
bool operator==(const deref_shared_ptr<T> &lhs,
                const deref_shared_ptr<U> &rhs) {
    return (*lhs == *rhs);
}
template <class T, class U>
bool operator!=(const deref_shared_ptr<T> &lhs,
                const deref_shared_ptr<U> &rhs) {
    return !(operator!=(lhs, rhs));
}
template <class T>
bool operator==(const deref_shared_ptr<T> &lhs, std::nullptr_t) {
    return (reinterpret_cast<const std::shared_ptr<T> &>(lhs) == nullptr);
}
template <class T>
bool operator==(std::nullptr_t, const deref_shared_ptr<T> &rhs) {
    return (reinterpret_cast<const std::shared_ptr<T> &>(rhs) == nullptr);
}
template <class T>
bool operator!=(const deref_shared_ptr<T> &lhs, std::nullptr_t rhs) {
    return !(operator==(lhs, rhs));
}
template <class T>
bool operator!=(std::nullptr_t lhs, const deref_shared_ptr<T> &rhs) {
    return !(operator==(lhs, rhs));
}

/// An equivalent of std::make_shared for \ref deref_shared_ptr.
template< class T, class... Args >
deref_shared_ptr<T> make_deref_shared( Args&&... args ) {
    return std::make_shared<T>(args...);
}

/// Releases moves a pointer from \c std::unique_ptr to \c deref_shared_ptr
template <typename T>
deref_shared_ptr<T> unique2derefshared(std::unique_ptr<T> &p) {
    return deref_shared_ptr<T>(p.release());
}

#endif
