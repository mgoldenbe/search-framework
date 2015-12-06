#include "type.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <type_traits>

struct VisitorBase {
    template <class Algorithm>
    static void OnEvent1(Algorithm &alg) {(void)alg;}

    template <class Algorithm>
    static void OnEvent2(Algorithm &alg) {(void)alg;}
};

struct UserVisitor: public VisitorBase {
    template <class Algorithm>
    static typename Algorithm::ResultOfHandlingEvent1 OnEvent1(Algorithm &alg) {
        std::cout << "In VisitorForEvent1! data1 = " << alg.data1 << std::endl;
        return Algorithm::ResultOfHandlingEvent1::EVENT1_ACTION1;
    }
};

// Declares two things:
//     -- algorithm's data
//     -- return values for visitors
struct AlgorithmBase {
    enum class ResultOfHandlingEvent1 {EVENT1_ACTION1, EVENT1_ACTION2};
    enum class ResultOfHandlingEvent2 {EVENT2_ACTION1, EVENT2_ACTION2};
    int data1 = 5;
};

template <class Visitor>
struct Algorithm: AlgorithmBase {
    void run() {
        constexpr bool ReturnTypeFlagOfOnEvent1 =
            std::is_same<decltype(Visitor::OnEvent1(*this)),
                         AlgorithmBase::ResultOfHandlingEvent1>::value;
        handleEvent1(std::integral_constant<bool, ReturnTypeFlagOfOnEvent1>());

        constexpr bool ReturnTypeFlagOfOnEvent2 =
            std::is_same<decltype(Visitor::OnEvent2(*this)),
                         AlgorithmBase::ResultOfHandlingEvent2>::value;
        handleEvent2(std::integral_constant<bool, ReturnTypeFlagOfOnEvent2>());
    }

private:
    void handleEvent1(std::true_type) {
        auto res = Visitor::OnEvent1((AlgorithmBase &)*this);
        std::cout << "Will switch on the result of handling event 1!"
                  << std::endl;
        (void)res; // but a switch statement will appear here
    }
    void handleEvent1(std::false_type) {
        Visitor::OnEvent1((AlgorithmBase &)*this);
        std::cout << "Nothing to be done for the result of handling event 1!"
                  << std::endl;
    }

    void handleEvent2(std::true_type) {
        auto res = Visitor::OnEvent2((AlgorithmBase &)*this);
        std::cout << "Will switch on the result of handling event 2!"
                  << std::endl;
        (void)res; // but a switch statement will appear here
    }
    void handleEvent2(std::false_type) {
        Visitor::OnEvent2((AlgorithmBase &)*this);
        std::cout << "Nothing to be done for the result of handling event 2!"
                  << std::endl;
    }
};


int main() {
    Algorithm<UserVisitor> a;
    a.run();
    return 0;
}
