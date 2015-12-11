#include <iostream>
#include <sstream>
#include "Node2Stream.h"
#include "NodesDiff.h"

struct NodeStatBase {
    using Base1 = void;
    REFLECTABLE()
};

struct NodeBase: public NodeStatBase {
    using Base1 = NodeStatBase; using Base2 = void;
    int h;
    REFLECTABLE((double)g, (double)f)
};

struct NodeData : public NodeBase {
    using Base1 = NodeBase; using Base2 = void;
    //REFLECTABLE()
    REFLECTABLE((double)F)
};

int main() {
    NodeData n1; n1.g = 2.2; n1.f = 3.0; n1.F = 2.0; //n1.nExp = 2;
    NodeData n2; n2.g = 1.5; n2.f = 4.0; n2.F = 5.0; //n2.nExp = 5;
    std::cout << n1 << std::endl;
    std::cout <<  nodesDiff(n1, n2) << std::endl;

    //NodeBase nb1; nb1.g = 1.0; nb1.f = 3.0;
    //NodeBase nb2; nb2.g = 1.0; nb2.f = 4.0;
    //std::cout <<  nodesDiff(nb1, nb2) << std::endl;

    return 0;
}
