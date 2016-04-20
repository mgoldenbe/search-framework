///@file
///@brief INTERFACES CHECKED.

#ifndef NODE_2_STREAM_H
#define NODE_2_STREAM_H

#include "ReflectStruct.h"

struct print_visitor {
    template<typename CharT, class FieldData>
    void operator()(FieldData f, std::basic_ostream<CharT> &o, int &index) {
        if (index++) o << "  ";
        o << f.name() << ": " << f.get();
    }
};

template <class Node, typename CharT>
void print_fields(Node &n, std::basic_ostream<CharT> &o, int &index) {
    auto a = std::bind(print_visitor(), std::placeholders::_1, std::ref(o),
                       std::ref(index));
    visit_each(n, a);
}

template <class Node>
std::string
nodeStr(const Node &n,
        typename std::enable_if<std::is_same<typename Node::Base1, void>::value,
                                int>::type = 0) {
    int index = 0;
    std::ostringstream so;
    so << "(";
    print_fields(n, so, index);
    so << ")";
    return so.str();
}

template <class Node>
std::string nodeStr(const Node &n, typename std::enable_if<
                        !std::is_same<typename Node::Base1, void>::value &&
                            std::is_same<typename Node::Base2, void>::value,
                        int>::type = 0) {
    std::string res = "(";
    res += nodeStr((typename Node::Base1 &)n);
    res.erase(0, 1);
    res.erase(res.size()-1, 1);

    int index = 0;
    std::ostringstream so;
    print_fields(n, so, index);
    if (so.str().size() > 0 && res.size() > 1) res += "  ";
    res += so.str();

    res += ")";
    return res;
}

template <typename CharT, class Node>
std::basic_ostream<CharT> &
dumpNode(std::basic_ostream<CharT> &o, const Node &n,
         typename std::enable_if<
             std::is_same<typename Node::Base1, void>::value, int>::type = 0) {
    return o << nodeStr(n);
}

template <typename CharT, class Node>
std::basic_ostream<CharT> &dumpNode(
    std::basic_ostream<CharT> &o, const Node &n,
    typename std::enable_if<!std::is_same<typename Node::Base1, void>::value &&
                                std::is_same<typename Node::Base2, void>::value,
                            int>::type = 0) {
    return o << nodeStr(n);
}

template <typename charT, class Node>
auto operator<<(std::basic_ostream<charT> &o, const Node &n)
    -> decltype(dumpNode(o, n)) {
    return dumpNode(o, n);
}

#endif
