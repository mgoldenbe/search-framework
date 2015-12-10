#include <iostream>
#include <sstream>
#include <functional>
#include "../utilities.h"
#include "ReflectStruct.h"

struct print_visitor {
    template<typename CharT, class FieldData>
    void operator()(FieldData f, std::basic_ostream<CharT> &o, int &index) {
        if (index++) o << "  ";
        o << f.name() << ": " << f.get();
    }
};

template <class T, typename CharT>
void print_fields(T &x, std::basic_ostream<CharT> &o, int &index) {
    auto a = std::bind(print_visitor(), std::placeholders::_1, std::ref(o),
                       std::ref(index));
    visit_each(x, a);
}

template <typename CharT, class Node>
std::basic_ostream<CharT> &
dumpNode(std::basic_ostream<CharT> &o, const Node &n,
         typename std::enable_if<
             std::is_same<typename Node::Base1, void>::value, int>::type = 0) {
    int index = 0;
    o << "(";
    print_fields(n, o, index);
    o << ")";
    return o;
}

template <typename CharT, class Node>
std::basic_ostream<CharT> &dumpNode(
    std::basic_ostream<CharT> &o, const Node &n,
    typename std::enable_if<!std::is_same<typename Node::Base1, void>::value &&
                                std::is_same<typename Node::Base2, void>::value,
                            int>::type = 0) {
    int index = 0;
    o << "(";
    print_fields(n, o, index);
    print_fields((typename Node::Base1 &)n, o, index);
    o << ")";
    return o;
}

template <typename charT, class Node>
auto operator<<(std::basic_ostream<charT> &o, const Node &n)
    -> decltype(dumpNode(o, n)) {
    return dumpNode(o, n);
}
