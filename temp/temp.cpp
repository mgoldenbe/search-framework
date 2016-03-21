#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cassert>
#include <functional>
#include <algorithm>
#include <chrono>
#include "PrettyPrint.h"

int main()
{
    auto begin = std::chrono::system_clock::now();
    std::cout << 5 << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(
                     std::chrono::system_clock::now() - begin).count()
              << std::endl;
    return 0;
}
