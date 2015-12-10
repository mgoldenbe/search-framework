#include <iostream>
#include <sstream>
#include <functional>
#include "../utilities.h"
#include "ReflectStruct.h"

struct second_node_visitor {
    template <typename CharT, class FieldData1, class FieldData2>
    void operator()(FieldData1 f1, FieldData2 f2, std::basic_ostream<CharT> &o,
                    int &index) {
        if (f1.name() == f2.name() && f1.get() != f2.get()) {
            if (index++) o << "  ";
            o << f1.name() << ": " << f1.get() << "-->" << f2.get();
        }
    }
};

struct first_node_visitor {
    template <typename CharT, class FieldData2, class Node>
    void operator()(FieldData2 f2, Node &n1, std::basic_ostream<CharT> &o,
                    int &index) {
        auto a = std::bind(second_node_visitor(), std::placeholders::_1, f2,
                           std::ref(o), std::ref(index));
        visit_each(n1, a);
    }
};

template <class Node, typename CharT>
void print_fields_comparison(Node &n1, Node &n2, std::basic_ostream<CharT> &o,
                             int &index) {
    auto a = std::bind(first_node_visitor(), std::placeholders::_1,
                       std::ref(n1), std::ref(o), std::ref(index));
    visit_each(n2, a);
}

template <class Node>
std::string
nodesDiff(const Node &n1, const Node &n2,
          typename std::enable_if<
              std::is_same<typename Node::Base1, void>::value, int>::type = 0) {
    int index = 0;
    std::ostringstream so;
    so << "(";
    print_fields_comparison(n1, n2, so, index);
    so << ")";
    return so.str();
}

template <class Node>
std::string nodesDiff(
    const Node &n1, const Node &n2,
    typename std::enable_if<!std::is_same<typename Node::Base1, void>::value &&
                                std::is_same<typename Node::Base2, void>::value,
                            int>::type = 0) {
    int index = 0;
    std::ostringstream so;
    so << "(";
    print_fields_comparison(n1, n2, so, index);
    print_fields_comparison((typename Node::Base1 &)n1,
                            (typename Node::Base1 &)n2, so, index);
    so << ")";
    return so.str();
}
