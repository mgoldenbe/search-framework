#ifndef UTIL_MEMORY
#define UTIL_MEMORY

///@file
///@brief Memory-related utilities.
/// \author Meir Goldenberg

/// Releases moves a pointer from \c std::unique_ptr to \c std::shared_ptr.
/// \tparam T The type being pointed to.
/// \param p The pointer to be released.
/// \return The shared pointer to the same object.
template <typename T>
std::shared_ptr<T> unique2shared(std::unique_ptr<T> &p) {
    return std::shared_ptr<T>(p.release());
}

/// Shared pointer whose \c operator== compares the pointed to values.
/// \tparam T The type being pointed to.
template <typename T>
class deref_shared_ptr: private std::shared_ptr<T> {
public:
    using Base = std::shared_ptr<T>; ///< The standard shared pointer.

    using Base::operator->;
    using Base::operator*;
    using Base::operator bool;
    using Base::reset;

    /// The default constructor.
    constexpr deref_shared_ptr() : Base() {}

    /// The constructor based on \c nullptr.
    constexpr deref_shared_ptr(std::nullptr_t) : Base(nullptr) {}

    /// The copy constructor.
    /// \param rhs The object being copied.
    deref_shared_ptr(const deref_shared_ptr &rhs) : Base(rhs) {}

    /// The constructor based on a pointer to a type that can be converted to
    /// \c T.
    /// \tparam Y A type that can be converted to T.
    /// \param ptr The pointer to be used for construction.
    template <class Y> explicit deref_shared_ptr(Y *ptr) : Base(ptr) {}

    /// The moving constructor based on a unique pointer to a type that can be
    /// converted to \c T.
    /// \tparam Y A type that can be converted to T.
    /// \tparam Deleter The deleter of the unique pointer type.
    /// \param r The unique pointer to be used for construction.
    template <class Y, class Deleter>
    deref_shared_ptr(std::unique_ptr<Y, Deleter> &&r) : Base(std::move(r)) {}

    /// The assignment operator.
    /// \param rhs The right-hand side of the assignment.
    deref_shared_ptr &operator=(const deref_shared_ptr &rhs) {
        this->Base::operator=(rhs);
        return *this;
    }

    /// The function similar to \c std::make_shared for \ref deref_shared_ptr.
    /// \tparam TT The pointed to type.
    /// \tparam Args The template argument list for the constructor.
    template <class TT, class... Args>
    friend deref_shared_ptr<TT> make_deref_shared(Args &&... args);

private:
    /// Conversion from a standard shared pointer.
    /// \param p The standard shared pointer to be converted.
    deref_shared_ptr(const Base &p) : Base(p) {}
};

/// Comparison operator for \ref deref_shared_ptr.
/// \tparam T The type pointed to by the left-hand side.
/// \tparam U The type pointed to by the right-hand side.
/// \param lhs The left-hand side of the comparison.
/// \param rhs The left-hand side of the comparison.
/// \return \c true if lhs==rhs and \c false otherwise.
template <class T, class U>
bool operator==(const deref_shared_ptr<T> &lhs,
                const deref_shared_ptr<U> &rhs) {
    return (*lhs == *rhs);
}

/// Comparison operator for \ref deref_shared_ptr.
/// \tparam T The type pointed to by the left-hand side.
/// \tparam U The type pointed to by the right-hand side.
/// \param lhs The left-hand side of the comparison.
/// \param rhs The left-hand side of the comparison.
/// \return \c true if lhs!=rhs and \c false otherwise.
template <class T, class U>
bool operator!=(const deref_shared_ptr<T> &lhs,
                const deref_shared_ptr<U> &rhs) {
    return !(operator!=(lhs, rhs));
}

/// Comparison operator for \ref deref_shared_ptr for the case of \c nullptr on the right.
/// \tparam T The type pointed to by the left-hand side.
/// \param lhs The left-hand side of the comparison.
/// \return \c true if lhs==nullptr and \c false otherwise.
template <class T>
bool operator==(const deref_shared_ptr<T> &lhs, std::nullptr_t) {
    return (reinterpret_cast<const std::shared_ptr<T> &>(lhs) == nullptr);
}

/// Comparison operator for \ref deref_shared_ptr for the case of \c nullptr on the left.
/// \tparam T The type pointed to by the right-hand side.
/// \param rhs The right-hand side of the comparison.
/// \return \c true if nullptr==rhs and \c false otherwise.
template <class T>
bool operator==(std::nullptr_t, const deref_shared_ptr<T> &rhs) {
    return (reinterpret_cast<const std::shared_ptr<T> &>(rhs) == nullptr);
}

/// Comparison operator for \ref deref_shared_ptr for the case of \c nullptr on the right.
/// \tparam T The type pointed to by the left-hand side.
/// \param lhs The left-hand side of the comparison.
/// \param rhs The right-hand side of the comparison.
/// \return \c true if lhs!=nullptr and \c false otherwise.
template <class T>
bool operator!=(const deref_shared_ptr<T> &lhs, std::nullptr_t rhs) {
    return !(operator==(lhs, rhs));
}

/// Comparison operator for \ref deref_shared_ptr for the case of \c nullptr on
/// the left.
/// \tparam T The type pointed to by the right-hand side.
/// \param lhs The left-hand side of the comparison.
/// \param rhs The right-hand side of the comparison.
/// \return \c true if nullptr!=rhs and \c false otherwise.
template <class T>
bool operator!=(std::nullptr_t lhs, const deref_shared_ptr<T> &rhs) {
    return !(operator==(lhs, rhs));
}

/// The function similar to \c std::make_shared for \ref deref_shared_ptr.
/// \tparam TT The pointed to type.
/// \tparam Args The template argument list for the constructor.
template< class T, class... Args >
deref_shared_ptr<T> make_deref_shared( Args&&... args ) {
    return std::make_shared<T>(args...);
}

/// Moves an object from \c std::unique_ptr to \ref deref_shared_ptr
/// \tparam T The pointed to type.
/// \param p The pointer to the object to be moved.
/// \return The resulting \ref deref_shared_ptr.
template <typename T>
deref_shared_ptr<T> unique2derefshared(std::unique_ptr<T> &p) {
    return deref_shared_ptr<T>(p.release());
}

#endif
