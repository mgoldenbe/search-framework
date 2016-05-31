#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

/// \file
/// \brief The \ref CommandLine class.
/// \author Meir Goldenberg

/// Short-hand notation for global access to the command line object.
#define CMD CommandLine::instance()

/// Class handling the command line. It is singleton. The command line would
/// usually be accessed from the other code using the \ref CMD macro.
struct CommandLine {
    /// Access to the command line object. When this method is called for the
    /// first time, it should be called with proper (i.e. not default) \c argc
    /// and \c argv.
    /// \param argc Number of command line arguments.
    /// \param argv The command line arguments.
    /// \return The command line object.
    static CommandLine &instance(int argc = 0, char **argv = nullptr) {
        static CommandLine res(argc, argv);
        return res;
    }

    /// Returns the name of file from which problem instances are to be read.
    /// \return The name of file from which problem instances are to be read.
    std::string instancesFileName() { return instancesFileName_.getValue(); }

    /// Returns \c true if an explicit domain is to be read from a file and \c
    /// false otherwise.
    /// \return \c true if an explicit domain is to be read from a file and \c
    /// false otherwise.
    bool spaceInitFileName_isSet() { return spaceInitFileName_.isSet(); }

    /// Returns the name of the file from which the explicit domain is to be
    /// read.
    /// \return The name of the file from which the explicit domain is to be
    /// read.
    std::string spaceInitFileName() { return spaceInitFileName_.getValue(); }

    /// Returns the number of problem instances.
    /// \return The number of problem instances.
    int nInstances() { return nInstances_.getValue(); }

    /// Returns the instance number for which the visualizer is to be run. If
    /// visualization is not to be performed, the function returns -1.
    /// \return The instance number for which the visualizer is to be run or -1
    /// if
    /// visualization is not to be performed.
    int visualizeInstance() { return visualize_.getValue(); }

    /// Returns the number of start states in a problem instance.
    /// \param mode \c 'r' or 'w' depending on whether the problem instances
    /// file is being read or written, respectively.
    /// \return The number of start states in a problem instance.
    int nStarts(char mode) {
        if (nStarts_.isSet()) return nStarts_.getValue();
        return (mode == 'w' ? 1 : INT_MAX);
    }

    /// Returns the number of goal states in a problem instance.
    /// \param mode \c 'r' or 'w' depending on whether the problem instances
    /// file is being read or written, respectively.
    /// \return The number of goal states in a problem instance.
    int nGoals(char mode) {
        if (nGoals_.isSet()) return nGoals_.getValue();
        return (mode == 'w' ? 1 : INT_MAX);
    }

    /// Returns \c true if per-problem-instance statistics are to appear on the
    /// output and \c false otherwise.
    /// \return \c true if per-problem-instance statistics are to appear on the
    /// output and \c false otherwise.
    bool perInstance() { return perInstance_.getValue(); }

    /// Returns \c true if the title row should not appear on the output and \c
    /// false otherwise.
    /// \return \c true if the title row should not appear on the output and \c
    /// false otherwise.
    bool hideTitle() { return hideTitle_.getValue(); }

    /// Returns the string with which the title row is to be prefixed.
    /// \return The string with which the title row is to be prefixed.
    std::string prefixTitle() { return prefixTitle_.getValue(); }

    /// Returns the string with which each data row is to be prefixed.
    /// \return The string with which each data row is to be prefixed.
    std::string prefixData() { return prefixData_.getValue(); }

private:
    /// The underlying TCLAP object.
    TCLAP::CmdLine cmd_;

    /// Command line option for the name of file from which problem instances
    /// are to be read.
    TCLAP::ValueArg<std::string> instancesFileName_;

    /// Command line option for the name of the file from which the explicit
    /// domain is to be read.
    TCLAP::ValueArg<std::string> spaceInitFileName_;

    /// Command line option for the number of problem instances.
    TCLAP::ValueArg<int> nInstances_;

    /// Command line option for the instance number for which the visualizer is
    /// to be run. It would hold -1 if visualization is not to be performed.
    TCLAP::ValueArg<int> visualize_;

    /// Command line option for the number of start states in a problem
    /// instance.
    TCLAP::ValueArg<int> nStarts_;

    /// Command line option for the number of goal states in a problem
    /// instance.
    TCLAP::ValueArg<int> nGoals_;

    /// Command line option for the switch showing whether per-problem-instance
    /// statistics are to appear on the output.
    TCLAP::SwitchArg perInstance_;

    /// Command line option for the switch showing whether the title row is to
    /// appear on the output.
    TCLAP::SwitchArg hideTitle_;

    /// Command line option for the string with which the title row is to be
    /// prefixed.
    TCLAP::ValueArg<std::string> prefixTitle_;

    /// Command line option for the string with which each data row is to be
    /// prefixed.
    TCLAP::ValueArg<std::string> prefixData_;

    /// Initializes the command line given \c argc and \c argv.
    /// \param argc Number of command line arguments.
    /// \param argv The command line arguments.
    CommandLine(int argc, char **argv)
        : cmd_("The Generic Search Library", ' ', "0.1"),
          instancesFileName_("i", "instances", "File with instances", true, "",
                             "string", cmd_),
          spaceInitFileName_("s", "space",
                             "File to initialize an explicit space", false, "",
                             "string", cmd_),
          nInstances_("n", "nInstances", "Number of instances to create", false,
                      -1, "int", cmd_),
          visualize_("v", "visualize", "Instance to visualize", false, -1,
                     "int", cmd_),
          nStarts_("", "nStarts", "Number of start states in an instance",
                   false, -1, "int", cmd_),
          nGoals_("", "nGoals", "Number of goal states in an instance", false,
                  -1, "int", cmd_),
          perInstance_("p", "perInstance", "Output per-instance stats", cmd_,
                       false),
          hideTitle_("", "hideTitle", "Do not show the title line", cmd_,
                     false),
          prefixTitle_("", "prefixTitle", "String to prefix the title row with",
                       false, "", "string", cmd_),
          prefixData_("", "prefixData",
                      "String to prefix the each data row with", false, "",
                      "string", cmd_) {
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
