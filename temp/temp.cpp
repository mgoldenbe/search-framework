#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cassert>
#include <functional>
#include <algorithm>
#include <chrono>
#include "PrettyPrint.h"
#include <tclap/CmdLine.h>

int main(int argc, char** argv)
{
    try {
        TCLAP::CmdLine cmd("Command description message", ' ', "0.9");
        TCLAP::ValueArg<std::string> nameArg("n", "name", "Name to print", true,
                                             "homer", "string");
        cmd.add(nameArg);
        TCLAP::SwitchArg reverseSwitch("r", "reverse", "Print name backwards",
                                       cmd, false);
        cmd.parse(argc, argv);

        std::cout << nameArg.getValue() << std::endl;
        std::cout << reverseSwitch.getValue() << std::endl;
    }
    catch (TCLAP::ArgException &e) // catch any exceptions
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId()
                  << std::endl;
    }
}
