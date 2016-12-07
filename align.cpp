
/// \file
/// \brief The tool for aligning output of several invocations of the framework.
/// \author Meir Goldenberg

#include "outside_headers.h"
#include "core/util/stream.h"
#include "core/util/container.h"
#include "core/util/string.h"
#include "core/util/table.h"

/// The main function.
int main(int argc, char **argv) {
    if (argc < 2)
        throw std::invalid_argument("File name is not specified");
    std::ifstream fin(argv[1]);
    if (!fin)
        throw std::invalid_argument("Could not open the file");
    std::string line;
    Table t;
    while (std::getline(fin, line)) {
        std::string s;
        while ((s = stuff(line)) != "")
            t << s;
        t << std::endl;
    }
    std::ofstream fout(argv[1]);
    fout << t;
    return 0;
}
