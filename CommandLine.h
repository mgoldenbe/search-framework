#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#define CMD CommandLine::instance()

// http://stackoverflow.com/a/7712341/2725810
struct CommandLine {
    // Should be called once at the beginning with proper argc and argv
    static CommandLine &instance(int argc = 0, char **argv = nullptr) {
        static CommandLine res(argc, argv);
        return res;
    }

    std::string instancesFileName() { return instancesFileName_.getValue(); }

    bool spaceInitFileName_isSet() { return spaceInitFileName_.isSet(); }
    std::string spaceInitFileName() { return spaceInitFileName_.getValue(); }

    int nInstances() { return nInstances_.getValue(); }

    int nStarts(char mode) {
        if (nStarts_.isSet()) return nStarts_.getValue();
        return (mode == 'w' ? 1 : INT_MAX);
    }

    int nGoals(char mode) {
        if (nGoals_.isSet()) return nGoals_.getValue();
        return (mode == 'w' ? 1 : INT_MAX);
    }

    bool perInstance() { return perInstance_.getValue(); }

private:
    TCLAP::CmdLine cmd_;
    TCLAP::ValueArg<std::string> instancesFileName_;
    TCLAP::ValueArg<std::string> spaceInitFileName_;
    TCLAP::ValueArg<int> nInstances_;
    TCLAP::ValueArg<int> nStarts_, nGoals_;
    TCLAP::SwitchArg perInstance_;

    CommandLine(int argc, char **argv)
        : cmd_("The Generic Search Library", ' ', "0.1"),
          instancesFileName_("i", "instances", "File with instances", true, "",
                             "string", cmd_),
          spaceInitFileName_("s", "space",
                             "File to initialize an explicit space", false, "",
                             "string", cmd_),
          nInstances_("n", "nInstances", "Number of instances to create", false,
                      -1, "int", cmd_),
          nStarts_("", "nStarts", "Number of start states in an instance",
                   false, -1, "int", cmd_),
          nGoals_("", "nGoals", "Number of goal states in an instance",
                   false, -1, "int", cmd_),
          perInstance_("p", "perInstance", "Output per-instance stats", cmd_,
                       false) {
        try {
            cmd_.parse(argc, argv);
        }
        catch (TCLAP::ArgException &e) // catch any exceptions
            {
                std::cerr << "error: " << e.error() << " for arg " << e.argId()
                          << std::endl;
            }
    }
};

#endif
