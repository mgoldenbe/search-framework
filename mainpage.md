[TOC]

# Motivation {#s-motivation}
The motivation of this project is to produce a framework to facilitate:

- Producing flexible and efficient implementations of search algorithms (See \ref s-flexibility-efficiency).
- Gaining insights into the properties of the search algorithms through a variety of means (each of these means will be detailed below):
    + Analyzing the log of events produced by the algorithm (See \ref s-examples).
    + Watching the progress of the algorithm in motion (See \ref s-examples).
    + Using policy-based design.

In addition, the visualization abilities of the framework can be put to good use for teaching heuristic search to students. The following sections detail each of the above items.

# Foundation principles
## Flexibility vs. efficiency {#s-flexibility-efficiency}
When we talk about a search algorithm such as *A\**, we usually talk about a whole large family of algorithms. These algorithms differ in many aspects: from storing different information with search nodes to handling various search conditions (e.g. inconsistency) in different ways. The programming techniques that address the need for flexibility involve a trade-off between flexibility and efficiency. Efficiency may be an important factor for the following reasons:

- It allows performing experiments that involve solving large problem instances, a large number of problem instances, exploring large parameter spaces.
- It allows achieving the most fair comparison of algorithms by running each algorithm at its best.
- It allows to avoid comments from reviewers about non-state-of-the-art performance.

In the reminder of this section, I provide a brief description of the mentioned trade-off and describe the way chosen by the framework:

### The trade-off {#s-tradeoff}
The programming techniques that address the need for flexibility most directly are conditional (including switch) statements and *polymorphism*. Two types of polymorphism are distinguished: *dynamic* and *static*. The choice between these techniques 

Conditional (including switch) statements and dynamic polymorphism allow us to choose execution path based on conditions that are checked at run-time. This is convenient, since the resulting executable can be used for many different experimental scenarios (which can be specified on the command line, for example). However, these techniques incur performance cost. Besides the time needed to check conditions at run-time, conditional statements (and dynamic polymorphism, which boils down to conditional statements as well) preclude important compiler optimizations.

Static polymorphism (e.g. *templates* and compile-time techniques such as *tag dispatch* in `C++`) provide maximal performance at the cost of the need to re-compile the program for each new configuration. 

### The best of the two worlds {#s-configuration}
The framework chooses the middle path in the above trade-off. Namely, parts of the framework not directly related to producing experimental results (e.g. the tools for analysis), flexibility is achieved by run-time techniques. Where the quality of experimental results is at stake, the performance is considered critical and compile-time techniques are used. 

In the latter case of using compile-time techniques, flexibility is achieved by writing configuration files consisting of pre-processor `#define` statements. The symbols defined by these statements are typically used as default template arguments. This mechanism is complemented with several convenience features:

- The configuration file does not have to define all the symbols used in the code, but only those that are used (i.e. those that appear in the templates that get instantiated). A special tool is called from the `Makefile` to replace occurrences of other symbols by a special defined name (namely, `Nothing`, which is defined as an empty `struct`).

- Separate compilation is *not* used, which means that there is only one `.cpp` file that contains the `main` function. This results in very short compile times. On my computer, the current version compiles in about seven seconds. (This decision is also forced by heavy use of templates, most of which accept the search domain as a template argument, sometimes indirectly). 

- One can compile the framework and run an experiment by a single command, in which one can specify the configuration file, additional symbols for compile-time configuration and command line arguments for run-time configuration. This makes it easy to write scripts that explore a parameter space and perform and experiment for each parameter setting. For example, consider the following command:

	  (make run MODE=debug CPP=test.cpp CONFIG="projects/KGoal/ConfigMinHeuristic.h" ADD="-DSLB_STATE='GridMapState<>'" OPT="-s ost001d.map8 -i ost001d.txt --nGoals 2 -v 50") 2>err

	Here, an experiment is being run (the `run` target is being invoked) after being compiled with debugging symbols and without optimizations (the `debug` mode; the alternative is the `production` mode). The `.cpp` file is specified next. Then we specify the configuration file and additional symbols for compile-time configuration. Lastly, command line arguments for run-time configuration appear. The output of `make` is sent to `standard error`, while the output of the framework will appear on `standard output`.

- The executables are cached. This allows the script for parameter space exploration to ignore the differences between parameters that relate to compile-time and run-time configuration. When the same compile-time configuration is encountered, an `md5sum` of the source code and the compile-time configuration (after the removal of the unused symbols) is quickly computed and the stored executable is run. (The idea is similar to the well-known `ccache` tool, but tailored to the needs of the framework.) 

## Gaining understanding through policy-based design {#s-policy}
[Policy-based design](https://en.wikipedia.org/wiki/Policy-based_design) is a notion popularized by Alexandr Alexandrescu in his seminal work [Modern C++ Design](https://en.wikipedia.org/wiki/Modern_C%2B%2B_Design). The following paragraph from the [Wikipedia article](https://en.wikipedia.org/wiki/Policy-based_design) provides a brief description of policy-based design (the italics are mine):
> The central idiom in *policy-based design* is a class template (called the *host class*), taking several type parameters as input, which are instantiated with types selected by the user (called *policy classes*), each implementing a particular implicit interface (called a *policy*), and encapsulating some *orthogonal* (or mostly orthogonal) aspect of the behavior of the instantiated host class. By supplying a host class combined with a set of different, canned implementations for each policy, a library or module can support an *exponential number of different behavior combinations*, resolved at *compile time*, and selected by mixing and matching the different supplied policy classes in the instantiation of the host class template. Additionally, by writing a custom implementation of a given policy, a policy-based library can be used in situations requiring behaviors *unforeseen by the library implementor*. Even in cases where no more than one implementation of each policy will ever be used, decomposing a class into policies can aid the design process, by increasing modularity and *highlighting exactly where orthogonal design decisions have been made*.
Suppose that we use policy-based design to implement the *A\** search algorithm. The class implementing the algorithm becomes the *host class*. Now, we need to identify orthogonal behaviors that distinguish the various variants of the algorithm. Each such behavior becomes a *policy*. Each way to implement a policy becomes a *policy class*. For one example, we might have a policy for dealing with inconsistency and implement two policy classes: the `do nothing` policy and the `bi-directional pathmax (BPMX)` policy.
The proposition I would like to make is that the work on identifying a set of policies (or a hierarchy of policies), we (as a research community) can arrive at a natural taxonomy of variants of *A\** and other search algorithms, which will contribute greatly to our understanding of both the state of the art and the research possibilities ahead. 

Building a taxonomy as described above is a long-term (and, hopefully, communal) effort. Therefore, I did not make an attempt at specifying all the policies of *A\**. Rather, the policies will be added and reviewed as the different variants of *A\** and other algorithms are added to the framework.

# Usage examples {#s-examples}
## A* for grid-based path-finding {#s-example-astar}
## IDA* for an implicit domain with automatic state graph layout {#s-example-ida}
## For teaching: the worst case for A* with an inconsistent heuristic {#s-example-teaching}

# Conventions {#s-concepts}
This section describes the conventions knowing which will help the reader to get started on working with the framework. 

## The usage of pre-processor symbols {#s-symbols}
Yes, I know, Scott Meyers states in `Item 2` of [Effective C++](http://amzn.com/0321334876) (3rd edition):
> It's not yet time to retire the preprocessor, but you should definitely give it long and frequent vacations.
Unfortunately, he is right not only in the second part of the quote, but also in the first part. I did not find any better way to define compile-time configuration than pre-processor symbols. As parenthetical note, the problem with `using` directives is that the types have to be defined before they can appear on the right side of such a directive.

Since we cannot retire the preprocessor, we need to make sure that it does not do harm along with the good. For this reason, the following conventions are proposed:
- All preprocessor symbols should have names starting with `SLB_` followed by capital letters and underscores. Besides avoiding clashes with other names, this allows for automatic substitution of unused symbols.
- The preprocessor symbols specific to an algorithm (i.e. that are not likely to be shared between several algorithms) should have the name of that algorithm in the name. This is meant as a (poor, but necessary) substitution for `namespace`s.

## Algorithmic events {#s-events}
The central idea making it possible to analyze the behavior of algorithms is that each algorithm can define events with a common interface. The log of such events can be both analyzed in textual mode and visualized. To make sure that no run-time overhead is incurred in production runs, we provide the \ref log function, which uses tag dispatch (which is a compile-time technique) to either do nothing or create and log the event. The reader is encouraged to see how events are logged in the existing code, e.g. in \ref Astar::run.

Each event defines a visualization effect. It is convenient to derive the events from pre-made bases, which correspond to different kinds of visualization effects. The existing bases are defined in \ref event_types.h and the existing events are defined in \ref events.h.

## Managed nodes {#s-nodes}
Managed nodes is another convenience that was achieved through the use of pre-processor. Defining the structure with the data stored by the search node using the `MANAGED` pre-processor symbol, provides reflection capabilities for that structure. In particular, this alleviates the need to define an output operator for that node. In addition, the framework is capable to compare the contents of two managed node and produce a string that summarizes the differences. These reflection capabilities come very handy for the textual log analysis tool. (The use of pre-processor to achieve reflection in `C++` is due to the author of [this](http://stackoverflow.com/a/11744832/2725810) post.)

# Currently defined configuration options
As explained in \ref s-configuration, the configuration options come in two flavors: compile-time and run-time. 

## Compile-time configuration options
The following pre-processor symbols are currently in use:

## Run-time configuration options
The following command line options are currently accepted (this list can also be produced by running the compiled `default_cpp.cpp` with the `--help` option:

# Installation {#s-install}
The installation instructions in this section have been tested on `Ubuntu 16.04`. The framework has not been tested for non-`Linux` environments. 

The following actions are needed to be able to run experiments: 
1. Download and extract the framework. Enter the framework's folder.
2. Make sure that the `COMPILER` variable in `Makefile` contains the correct path to version 4.9 or newer of `g++` (that's when `g++` began to fully support regular expressions). 
3. Install `xlib`:

	   sudo apt-get install libx11-dev
4. Install `cairo`:

	   sudo apt-get install libcairo2-dev
5. Make sure that the `CAIRO_PATH` variable in `Makefile` contains the correct path to `cairo.h`.
6. Download and extract the [Boost](http://www.boost.org/users/download/) library. 
7. Set the `BOOST_PATH` variable in `Makefile` to point to the `Boost` directory.
8. Install `ncurses`:

	   sudo apt-get install libncurses5-dev

# Other related software {#s-related}
The software frameworks written in `C++` most similar in purpose to my framework are [HOG2](https://github.com/nathansttt/hog2) by Nathan Sturtevant and the [Research Code for Heuristic Search](https://github.com/eaburns/search) by Ethan Burns. There is also the [Combinatorial Search for Java](https://github.com/matthatem/cs4j) written by Matthew Hatem in `Java`. All these frameworks aim for both performance and flexibility. In addition, `HOG2` and the `Research Code for Heuristic Search` are capable of producing visualizations, at least for some domains. However, all three frameworks lack proper documentation, so that one has to understand them from the source code. In addition, my framework provides the following features that are, to the best of my knowledge, not part of any existing framework:
  + Tools for analysis.
  + Visualization of non-grid domains, including automatic layouts.
  + Tools for supporting compile-time configuration and scripting.
  + The idea of gaining understanding through policy-based design is, to the best of my knowledge, also new in my framework.

Several other projects that are less related to this framework are:
- [OCaml Search Code](https://github.com/jordanthayer/ocaml-search) by Jordan Thayer. This framework is written in `OCaml` and has not been updated since 2012, when the group switched to `C++` for performance reasons.
- [The Boost Graph Library](http://www.boost.org/doc/libs/1_61_0/libs/graph/doc/). `BGL` provides several algorithms for searching in graphs. 
- [PSVN](https://era.library.ualberta.ca/downloads/7m01bn08g) by Robert Holte.
- [Fast Downward](http://www.fast-downward.org/) by Malte Helmert.

# Version and license information {#s-version}
\version 0.1
\author Meir Goldenberg (mgoldenbe@gmail.com)
\copyright MIT License
\warning Boost and Cairo come with their respective licences 

# Future work {#s-future}
This is a list of features I chose to not include in this version, but the need for which is already evident. The list does not follow any particular order:
- Ability to specify labels for vertices of the state graph.
- Limiting the time for computing a layout (it should be a command-line option), so that large state graphs can be displayed. Right not, the automatic layout computation does not scale beyond several hundred of vertices.
- Designing a way for algorithms to define their own command-line options. Currently, one has to modify the \ref CommandLine class to add command line options, which is obviously not ideal.
- The open list of A* can be made more efficient in several ways:
  + Using hints for re-inserting a node.
  + Keeping a hash of keys.
  + Using an underlying container other than `map`.
- Keeping a mapping from tiles to edges, vertices and any other drawing elements and also the inverse map, so that updates can be done cleanly, thus enabling richer graphical elements.
- Enabling `no graphics mode`.

# Notes to add
- Conditional compilation should be employed for both compilation efficiency and so the specific command-line options don't need to be defined if the related user-defined facilities aren't used.
- Forward declarations are used to enable user-defined facilities to be used as default template arguments in the core part of the framework.
- Additional command line options can be associated with user-defined facilities. Such options should have a forward declaration. Also, functions that read command line options should be templated using the CMD_TPARAM macro. They can then access the command line object using the CMD macro. 
