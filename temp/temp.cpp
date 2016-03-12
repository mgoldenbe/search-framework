#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cassert>
#include <functional>
#include <algorithm>
#include "PrettyPrint.h"

struct A {
    A()  {std::cout << x << std::endl;}
    int *x{};
};
int main()
{
    A a; (void)a;
    return 0;
}
