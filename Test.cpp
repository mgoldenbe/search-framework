#include "Pancake.h"
#include "OL.h"
#include "OCL.h"

extern constexpr double a = 0.2;

template <class Node>
using MyOpen = OL<Node>;

void testOL() {
    using Node = AStarNode<Pancake>;
    using NodeUP = std::unique_ptr<Node>;
    auto n1 = NodeUP(new Node(Pancake(0))); n1->f = 1;
    auto n2 = NodeUP(new Node(Pancake(0))); n2->f = 3; n2->g = 30;
    auto n3 = NodeUP(new Node(Pancake(0))); n3->f = 3; n3->g = 20;
    auto n4 = NodeUP(new Node(Pancake(0))); n4->f = 3; n4->g = 50;
    auto n5 = NodeUP(new Node(Pancake(0))); n5->f = 5;
    MyOpen<Node> ol;
    ol.add(n5.get());
    ol.add(n4.get());
    ol.add(n2.get());
    ol.add(n1.get());
    ol.add(n3.get());
    auto oldPriority = DefaultPriority<Node>(*n1);
    n1->f = 100;
    ol.update(n1.get(), oldPriority);
    ol.dump();
    for (auto i = 0U; i != 5; i++) {
        std::cout << "Greatest priority: " << ol.curPriority() << std::endl;
        std::cout << "Selected for expansion: " << *ol.deleteMin() << std::endl;
    }

    //std::cout << *n1 << std::endl;
}

void testOCL() {
    using Node = AStarNode<Pancake>;
    using NodeUP = std::unique_ptr<Node>;
    Pancake s1(5);
    Pancake s2(s1); s2.move(1);
    Pancake s3(s1); s3.move(2);
    Pancake s4(s1); s4.move(3);
    Pancake s5(s1); s5.move(4);
    auto n1 = NodeUP(new Node(s1)); n1->f = 1;
    auto n2 = NodeUP(new Node(s2)); n2->f = 2;
    auto n3 = NodeUP(new Node(s3)); n3->f = 3;
    auto n4 = NodeUP(new Node(s4)); n4->f = 4;
    auto n5 = NodeUP(new Node(s5)); n5->f = 5;
    OCL<OL<Node>> ocl;
    ocl.add(std::move(n5));
    ocl.add(std::move(n4));
    ocl.add(std::move(n2));
    ocl.add(std::move(n1));
    ocl.add(std::move(n3));

    auto myNode = ocl.getNode(s2);
    auto oldPriority = DefaultPriority<Node>(*myNode);
    myNode->f = 100;
    ocl.update(myNode, oldPriority);
    ocl.dump();

    for (auto i = 0U; i != 5; i++)
        std::cout << *ocl.minNode() << std::endl;

    //std::cout << *n1 << std::endl;
}

int main() {
    testOCL();
    return 0;
}
