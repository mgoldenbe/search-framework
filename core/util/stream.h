///@file
///@brief Stream output-related utilities.
/// \author Meir Goldenberg

#ifndef UTIL_STREAM_H
#define UTIL_STREAM_H

/// Get an arbitrary object's string representation.
/// \tparam T The object's type.
/// \param t The object.
/// \return The string representation of \c t.
template <typename T>
std::string str(const T& t) {
    std::ostringstream ss; ss << t; return ss.str();
}

/// Read from a file and assert that no errors occurred during the read.
/// \param fin The file input stream.
/// \return The string that was read.
std::string sureRead(std::ifstream &fin) {
    std::string res;
    fin >> res;
    assert(fin);
    return res;
}

/// Read from a file and assert that the read string is equal to the given one.
/// \param fin The file input stream.
/// \param str The given string.
/// \return The string that was read.
std::string sureRead(std::ifstream &fin, const std::string &str) {
    std::string res = sureRead(fin);
    assert(res == str); (void)str;
    return res;
}

/// Dump an object to \c stderr.
/// \tparam T The object type.
/// \param t The object.
template <typename T>
void dumpT(const T& t) {
    std::cerr << t << std::endl;
}

/// Output operator for all classes that define the dump member function
/// accepting \c std::basic_ostream.
/// \tparam charT The character type for \c std::basic_ostream.
/// \tparam T The type of the object to be output.
/// \return The updated output stream.
template <typename charT, typename T>
auto operator<<(std::basic_ostream<charT> &o, const T &t)
    -> decltype(t.dump(o)) {
    // Note that dump() member function can work for other things such as Table
    // If we were to not use dump(), there would be collisions with the output
    // operators of Table.
    return t.dump(o);
}

#endif
