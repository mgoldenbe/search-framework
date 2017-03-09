[TOC]

# 1 Motivation {#s-motivation}
The motivation of this project is to produce a framework to:

- Facilitate flexible and efficient implementations of search algorithms (See \ref s-flexibility-efficiency).
- Facilitate gaining of insights into properties of search algorithms through a variety of means:
    + Watching the progress of the algorithm in motion (See the [video demo](https://youtu.be/cElxLWve1Zw)).
    + Analyzing the log of events produced by the algorithm (See the [video demo](https://youtu.be/cElxLWve1Zw)).
    + Using policy-based design (See \ref s-policy).
- Enable easy production of examples that can be used for conference and classroom presentations.

The framework is written in `C++` (the `C++11` standard) to take advantage of both the expression power and the performance-related characteristics of this language.

# 2 Foundation principles {#s-principles}
## 2.1 Flexibility vs. efficiency {#s-flexibility-efficiency}
When we talk about a search algorithm such as *A\**, we usually talk about a whole large family of algorithms. These algorithms differ in many aspects: from storing different information with search nodes to handling various search conditions (e.g. inconsistency) in different ways. The programming techniques that address the need for flexibility involve a trade-off between flexibility and efficiency. Efficiency may be an important factor for the following reasons:

- It allows performing experiments that involve solving large problem instances, a large number of problem instances and exploring large parameter spaces.
- It allows achieving the most fair comparison of algorithms by running each algorithm at its best.
- It allows to avoid comments from reviewers about non-state-of-the-art performance.

In the remainder of this section, I provide a brief description of the mentioned trade-off and describe the way chosen by the framework.

### 2.1.1 The trade-off {#s-tradeoff}
The programming language mechanisms that address the need for flexibility most directly are conditional statements and *polymorphism*. Two types of polymorphism are distinguished: *dynamic* and *static*. 

Conditional statements and dynamic polymorphism allow us to choose execution path based on conditions that are checked at run-time. This is convenient, since the resulting executable can be used for many different experimental scenarios. However, these techniques incur performance cost. Only a small portion of this cost is due to the time taken to evaluate the conditions. A much greater performance cost may be incurred due to the fact that the need to check conditions at run-time precludes important compiler optimizations.

Static polymorphism (e.g. *templates* and compile-time techniques such as *tag dispatch*) provide maximal performance at the cost of the need to re-compile the program for each new configuration. 

### 2.1.2 The best of the two worlds {#s-configuration}
The framework chooses the middle path in the above trade-off. Namely, in the parts of the framework not directly related to producing experimental results (e.g. the tools for analysis), flexibility is achieved by run-time techniques. Where the time quality of experimental results is at stake, the performance is considered critical and compile-time techniques are used. 

In the latter case of using compile-time techniques, flexibility is achieved by enabling configuration files consisting of preprocessor symbol definitions (see \ref s-why-symbols for the justification of this design decision). With rare exceptions, each symbol determines a behavior of the framework and is used as a default template argument somewhere in the framework. The mechanism is complemented with several convenience features:

- The configuration file does not have to define all the symbols that appear in the framework, but only those that are actually used (i.e. those that appear in the templates that get *instantiated*). This is enabled by a special tool that takes over from the preprocessor and eliminates the unused symbols from the source before the actual compilation begins.

- Two features are aimed at making it easy and efficient to write scripts that explore a parameter space and perform an experiment for each parameter setting:
  - One can compile the framework and run an experiment by a single command, in which one can specify the configuration file, additional symbols for the *compile-time* configuration and command line arguments for the *run-time* configuration. 
  <!-- For example, consider the following command: -->

  <!-- 		(make run MODE=debug CPP=test.cpp CONFIG="projects/KGoal/ConfigMinHeuristic.h" ADD="-DSLB_STATE='GridMapState<>'" OPT="-s ost001d.map8 -i ost001d.txt --nGoals 2 -v 50") 2>err -->

  <!-- 	Here, an experiment is being run (the `run` target is being invoked) after being compiled with debugging symbols and without optimizations (the `debug` mode; the alternative is the `production` mode). The `.cpp` file is specified next. Then we specify the configuration file and additional symbols for compile-time configuration. Lastly, command line arguments for run-time configuration appear. The output of `make` is sent to `standard error`, while the output of the framework will appear on `standard output`. -->

  - The executables are cached. That is, if the current version of the framework's source code with the given compile-time configuration had ever been compiled, the executable from that previous compilation will be quickly retrieved and run. If not, the framework will be compiled and the executable will be saved for future use. The cached executables <!--(named after the `md5` sum of the source code with the unused symbols removed and the compile-time configuration)--> are stored in the `.execs/` directory.

The [video demo](https://youtu.be/cElxLWve1Zw) explains this mechanism using several sample scenarios.

## 2.2 Gaining understanding through policy-based design {#s-policy}
[Policy-based design](https://en.wikipedia.org/wiki/Policy-based_design) is a notion popularized by Andrei Alexandrescu in his seminal work [Modern C++ Design](https://en.wikipedia.org/wiki/Modern_C%2B%2B_Design). The following paragraph from the [Wikipedia article](https://en.wikipedia.org/wiki/Policy-based_design) provides a brief description of policy-based design (the italics are mine):
> The central idiom in *policy-based design* is a class template (called the *host class*), taking several type parameters as input, which are instantiated with types selected by the user (called *policy classes*), each implementing a particular implicit interface (called a *policy*), and encapsulating some *orthogonal* (or mostly orthogonal) aspect of the behavior of the instantiated host class. By supplying a host class combined with a set of different, canned implementations for each policy, a library or module can support an *exponential number of different behavior combinations*, resolved at *compile time*, and selected by mixing and matching the different supplied policy classes in the instantiation of the host class template. Additionally, by writing a custom implementation of a given policy, a policy-based library can be used in situations requiring behaviors *unforeseen by the library implementor*. Even in cases where no more than one implementation of each policy will ever be used, decomposing a class into policies can aid the design process, by increasing modularity and *highlighting exactly where orthogonal design decisions have been made*.
For example, suppose that we use policy-based design to implement the *A\** search algorithm. The class implementing the algorithm becomes the *host class*. Now, we need to identify orthogonal behaviors that distinguish the various variants of the algorithm. Each such behavior becomes a *policy*. Each way to implement a policy becomes a *policy class*. For one example, we might have a policy for dealing with inconsistency and implement two policy classes: the `do_nothing` policy and the `bidirectional_pathmax (BPMX)` policy.

The *proposition* I would like to make is that, by gradually identifying a set of orthogonal policies (actually, a hierarchy of policies, since each policy may in turn be a host class with its own policies), we, as a research community, can arrive at a natural taxonomy of the variants of *A\** and other search algorithms. I believe that such a taxonomy would contribute greatly to our understanding of both the state of the art and the research possibilities ahead. 

The algorithms that are already implemented in the framework show examples of using policy-based design to implement a heuristic search algorithm. Of course, examples cannot not serve as proofs of the concept's applicability to all the possible search algorithms. In fact, I do not think such a proof exists. What does exist is the world-wide programming community's experience of almost two decades of using policy-based design for successfully implementing complex systems with high demands for generality, flexibility and maintainability. However, anyone familiar with the existing body of search algorithms would realize that using this technology for heuristic search research will put this technology to a fascinating test.

Building a taxonomy as described above is a long-term (and, hopefully, communal) effort. Therefore, I did not attempt to arrive at a perfect set of policies for any algorithm. Rather, the policies will be added and refined as the different algorithms and variants are added to the framework.

# 3 Usage examples {#s-examples}
The framework can be used in a great variety of ways. In particular, it comes with the \ref test.cpp module that implements three modes of operation:
- **Building problem instances.** In this mode, a file of random problem instances and the associated user-defined properties is produced. %and saved in the file specified by the run-time configuration. Problem instances can be configured to have user-defined properties, which can be used for post-processing analysis using bucketing and other techniques.
- **Running a production experiment.** In this mode, the algorithm of interest is employed to solve the specified problem instances. The produced table of results contains user-defined statistics. If per-instance results are requested, these results are sorted based on user-defined criteria associated with the instance properties.
- **Analyzing the algorithm.** In this mode, the algorithm's execution for solving the specified problem instance is exposed by the facilities of the user interface. These facilities allow the researcher to explore the graphical representation of the search domain,\footnote{This representation is built by applying either an automatic or a user-defined layout.} visualize the search process, build filters and perform searches to focus on the parts of the search process that provide most insight, and more. 

One can also use screen shots or video captures of an analysis  session for presentation and teaching purposes. The [video demo](https://youtu.be/cElxLWve1Zw) shows examples of the framework's usage. When you download the framework, it will have all the files relevant to this demo in the `projects/demo directory`.

Lastly for this section, I would like to share an example of how the framework can help better understand search algorithms from a theoretical point of view. In a recent experimental study, it was noticed that two variants of A* resulted in vastly different sizes of the open list. None of the co-authors could think of an explanation. With the framework's help, we now have a video which shows that the *shapes* of the open lists produced by the two variants are very different. One variant produces a smoothly shaped open list, while the other one produces an open list with a large number of protrusions, which greatly increases the size of the open list. Once this direct observation was obtained, we were also able to understand why the algorithms' makeup resulted in this behavior.

# 4 Conventions {#s-concepts}
This section describes the conventions knowing which will help the reader to get started on working with the framework. 

## 4.1 The physical structure {#s-physical}
The framework is a header-only library. Let us explore the files comprising the framework.

### 4.1.1 The source code {#s-source}
The source code of the framework is organized into two directories:
- The `core/` directory contains the core facilities of the framework. In particular:
  - The files with the implementation of the user interface are stored in `core/user_interface/`.
  - The facilities that directly support the implementation of heuristic search algorithms are located in `core/search_base/`.
  - The lower-lever facilities are located in `core/util/`.
- The `extensions/` directory contains the facilities contributed by the users of the framework, i.e. the researchers. Usually, these facilities are directly supported by the facilities found in `core/search_base/`.
- The `projects/` directory is designated for the files related to specific experiments. The files may be of many kinds: configuration files, files storing the explicit domains, problem sets etc. At this point, each project gets its own directory in `projects/`, such as the `projects/demo/` directory used in the [video demo](https://youtu.be/cElxLWve1Zw). As the number of users grow, each user might get a directory in `projects/` named after him.
  
Lastly, the root directory contains: 
- The file \ref test.cpp, which is the default main module, which implements running an experiment with a single configuration (in either production or analysis mode) and building a file with problem instances. 
- The `align` utility, which is useful for aligning columns of the tables produced by several different invocations of the framework (see \ref s-single).
- The `symbols` utility, which is invoked automatically during the compilation to get rid of the unused preprocessor symbols as explained in \ref s-configuration.

### 4.1.2 The namespaces {#s-namespace}
All of the framework's facilities live in the `slb` namespace. In addition, the framework is organized in namespaces that mimic the folder structure. Thus, for example, the search base facilities are found in the \ref slb::core::sb namespace.

### 4.1.3 Header files for forward declarations {#s-forward}
As explained in \ref s-configuration, most of the framework's facilities are implemented as template classes, whose default template arguments are determined by the compile-time configuration. Thus, it is possible for a template that implements a core facility to have a default template argument, which is a user-provided extension. If that extension in turn uses the core facility in question, a circular dependency emerges. 

To avoid such circular dependencies, we need to forward-declare the names corresponding to the default template arguments. For this reason, the convention is that user-provided facilities should provide a header with forward declarations. This header's name should have the suffix `_fwd.h`. For example, the Pancake Puzzle domain comes in two headers: `extensions/domains/pancake.h` and `extensions/domains/pancake_fwd.h`. 
<!-- In particular, if additional command line options are defined, the class defining them should be forward-declared.  -->

The *forward-declaration headers* must be <code>\#include</code>-d in the final source before the core facilities as described in the next subsection.

### 4.1.4 The single-unit compilation model {#s-single-unit}
Since most of the facilities of the framework are template classes that have the search domain as one of their template parameters, separate compilation is difficult to achieve. Instead, the framework is compiled as a single translation unit. The following organization of <code>\#include</code>-directives makes this unit easy to manage:
- The main `.cpp` includes the configuration file (its name is given by the `CONFIG` symbol) and `slb.h`.
- `slb.h` includes:
  - `outside_headers.h`, which in turn includes all the third-party facilities (i.e. the facilities coming from the standard library, the `Boost` library etc.). This header is suitable for pre-compilation, which is currently not employed. <!-- I got only 25% speed-up from using pre-compilation. --> 
  - `extensions/headers_fwd.h`
  - `core/headers.h`
  - `extensions/headers.h`.
- For a directory `X/`: 
  - `X/headers_fwd.h` includes `headers_fwd.h` from each of the sub-direcories of `X`. 
  - `X/headers.h` includes: 
	- all headers from `X/` (besides `X/headers_fwd.h` and `X/headers.h`) and 
	- `headers.h` from each of the sub-direcories of `X/`. 

On my computer, the framework compiles in under ten seconds.

## 4.2 Compile-time configuration {#s-symbols}
The framework employs preprocessor symbols to enable compile-time configuration. Please refer to the [video demo](https://youtu.be/cElxLWve1Zw) for an initial explanation with some examples. This section provides some additional explanations.

### 4.2.1 Why preprocessor symbols? {#s-why-symbols}
`C++11` contains a number of features whose aim (among other things) is to make most previously needed uses of the preprocessor directives unnecessary. There are many strong reasons for that effort and listing those reasons is beyond the scope of this page. It will suffice to quote Scott Meyers, who states in `Item 2` of [Effective C++ (3rd edition)](http://amzn.com/0321334876):
> It's not yet time to retire the preprocessor, but you should definitely give it long and frequent vacations.

Unfortunately, Scott Meyers is right not only in the second part of the quote, but also in the first part. I did not find any better way to define compile-time configuration than preprocessor symbols. One obvious candidate for an alternative is to use aliases (i.e. the `using` keyword). One particular problem with aliases did not allow me to pursue that direction. Namely, there is no easy way to determine which aliases are unused. Therefore, each compile-time configuration would have to define all the aliases, even the ones not used in the current experiment. This also means that introducing a new alias would necessitate updating all the existing experiments' configurations. In contrast, once the preprocessor is done, the unused symbols remain in the code and can be recognized by an automatic tool. This is precisely what the framework's `symbols` tool does.


### 4.2.2 Names of the preprocessor symbols {#s-symbols-conventions}
The framework employs the following conventions regarding the names of the preprocessor symbols:
- The names are prefixed with `SLB_` and contain only capital letters and underscores.
- The names should be composed in a way that minimizes the probability of clashes with the names defined by other users. See the names of the existing user-defined symbols for examples of good names. <!-- The preprocessor symbols specific to an algorithm (i.e. that are not likely to be shared between several algorithms) should have the name of that algorithm in the name. This is meant as a (poor, but necessary) substitution for name scopes. -->

## 4.3 Run-time configuration {#s-run-config}
The framework employs the [Templatized C++ Command Line Parser Library](http://tclap.sourceforge.net/) to parse the command line. The following subsections present the framework's features related to using the command line.

### 4.3.1 Command line options for user-provided facilities {#s-user-cmd}
In addition to the standard command options (see below), the framework makes it possible to define command line options specific to user-provided facilities. For this reason, the class \ref slb::core::commandLine::CommandLine is a template that accepts a class implementing additional command line options as the template argument. See \ref slb::ext::domain::pancake::CommandLine for an example of such a class. Also, see the [video demo](https://youtu.be/cElxLWve1Zw) for an example of configuring the framework to use additional command line options.

One can view the standard command line options by running the following command from the root directory.

	(make run MODE=debug CPP=test.cpp CONFIG="projects/demo/grid.astar.h" OPT="--help")
This particular configuration file (i.e. `projects/demo/grid.astar.h`) was chosen because it does not define any additional command line options.

### 4.3.2 Using the command line object {#s-singleton}
The class \ref slb::core::commandLine::CommandLine is a singleton. To initialize the command line object, one must call the \ref slb::core::commandLine::CommandLine<>::instance function, forwarding to it the `argc` and `argv` arguments of `main`. Following the initialization, the command line object can be accessed by the user-provided facilities by calling the \ref slb::core::commandLine::CommandLine<>::instance function without arguments. The \ref CMD macro is defined as a short-hand for calling this function. 

There are two cases in which the command line object cannot be accessed by using the \ref CMD macro: 
- from the *core facilities* . This is because the classes implementing the user-provided additional command line options are only forward-declared (but not defined) by the time the compiler parses the core facilities. 
- in order to access command line arguments defined by a user-provided facility. This is because, when the user-provided facility is not used by the particular experiment, the associated command line addition is not defined.

In these cases, the command line object needs to be made available through a template argument. The framework provides the macros \ref CMD_TPARAM and \ref CMD_T are provided for this purpose. For example, here is how the constructor of the Pancake's state accesses the number of pancakes supplied on the command line:

~~~~~~~~~~~~~~~{.c}
template <CMD_TPARAM>
Pancake() : pancakes_(CMD_T.nPancakes()) {...}
~~~~~~~~~~~~~~~

## 4.4 Producing a table of results from several invocations {#s-single}
When exploring a parameter space by means of a script, it may be convenient to present the results of several invocations of the framework in a single table. The framework supports this by providing several standard command line options and a tool:
- The `--prefixTitle` option can be used to prepend the *title row* with a given string. The `--prefixData ` option can be used to prepend all the *data rows* with a given string. The script can use these options to insert extra columns into the table output by the framework. The purpose of these columns is to identify the parameter settings specific to a particular invocation of the framework. 
- The script can use the `--hideTitle` switch to append the table of results from the first invocation of the framework by only the data rows from the consequent invocations. Namely, when the `--hideTitle` switch is specified, the title row is not shown in the output. 
- The `align` tool can be used to align the columns of the resulting table. This makes it easy to import the table correctly into a spreadsheet. The tool takes a file name containing a table of results as a command line argument and aligns the table in place.


## 4.5 Algorithms, policies and communication between them {#s-crtp}
The currently implemented algorithms employ a design solution that advocates for keeping algorithms as simple as possible without compromising generality. In particular:
- An algorithm should implement only the core flow of the search. The specific behaviors are to be factored out into policies.
- Communications between algorithms and policies should follow a similar pattern. 
- Common characteristics of algorithms should be factored out into abstract base algorithms, from which specific algorithms can inherit.

In addition, there are cases when different policies of an algorithm need to communicate with each other. The proposed design supports this communication as well. 

The following subsections describe a design solution that supports the above requirements. For the sake of uniformness, the users are strongly advised to employ this solution for their implementations. 

### 4.5.1 Policy services {#s-services}
Each search algorithm provides *services* for its policies. These *policy services* are public member functions of the class implementing the algorithm. Implementations of policy services can be shared between several algorithms by factoring these services out into the abstract base algorithm from which the algorithms in question inherit.

### 4.5.2 Communication through reference to algorithm {#s-communication}
Each class implementing a policy of a search algorithm has a constructor that accepts an algorithm reference argument and stores this reference for future use. This way, function members of the policies have an empty parameter list, but can call the services provided by the algorithm. Consequently, policy classes are templates, where the template parameter is the class implementing the search algorithm.

This design provides an easy way of communication between policies as well. The algorithm can support this communication by providing policy services that return references to policy classes.

### 4.5.3 CRTP to enable initialization of policy classes in the abstract base algorithms {#s-base-crtp}
Using policy-based design in conjunction with abstract base algorithms as described above presents the following problem. Some policies are present in many algorithms. For example, many algorithms might have a policy for evaluating the stopping condition and maintaining the relevant data. When we factor out abstract base algorithms, these common policies become policies of the base algorithm. However, we would like these policies to have access to all the policy services provided by the actual (non-abstract) algorithms. Hence, the template classes implementing these policies need to be instantiated with the concrete algorithm's type, not the base algorithm's one.

To solve this problem, we use the *Curiously Recurring Template Pattern* (CRTP), whereby the abstract base algorithm has a template parameter specifying the concrete algorithm. Describing this technique is beyond the scope of this short document. The reader is referred to the [Wikipedia article](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern) for details. <!--I also used [this](http://stackoverflow.com/a/35436437/2725810) and [this](http://stackoverflow.com/a/5534818/2725810) posts for further clarifications.-->

### 4.5.4 Compile-time checks to avoid unneeded branching {#s-branching}
It is known that branching precludes significant compiler optimizations, so that avoiding unnecessary branching in implementations of search algorithms is an important efficiency concern.

Let us consider a particular example of such branching. Suppose that, in some variant of A* (and there are such variants), the selected node is not necessarily expanded. Furthermore, suppose that the search algorithm has a policy whose member function is called upon node selection. The regular implementation of this function does not return a value. However, the implementation for the mentioned variant returns a boolean value that indicates whether the selected node should be expanded or not. Depending on the particular policy implementation chosen by the compile-time configuration, the call to the policy's member function does or does not need to be followed by branching in the algorithm. 

We would like to make sure that no branching occurs in the latter case. Hence, we require that policy implementations provide information to enable compile-time checks to avoid such unnecessary branching.

### 4.5.5 Policy-based design and efficiency {#s-policy-efficiency}
Policy-based design uses the template mechanism of C++. A distinguishing characteristic of C++ templates is that, once a template is instantiated with particular policies, the resulting code preserves complete type information and is not a small bit worse than the code without templates that has the same policies hard-coded. In particular, using this mechanism enables compiler optimizations that depend on the availability of type information.

However, there is no claim that it is easy to achieve state-of-the-art performance in practice. It is not easy and may not even be practicable. First, one needs to be careful in how he chooses and implements the policies. Second, the optimizer in a modern C++ compiler is very complex and there aren't many people who can really predict how a particular implementation will be optimized. Another level of complexity is contributed by the intricacy of the modern hardware. Performance may be affected by seemingly irrelevant details, such as the relative order of interchangeable statements.

I have performed an experiment, whereby the performance of the framework's generic implementation of IDA* for solving instances of the sliding-puzzle domain are compared to the performance of the well-known highly crafted code of Richard Korf. The framework implements \ref slb::ext::domain::sliding_tile::SlidingTile "the domain" in such a way that the nodes are generated in the same order as they are generated by the code of Richard Korf.  Both the code of Korf and the framework were compiled with the GNU compiler using the highest optimization level (the \c -O3 switch). On my computer, the highly crafted code of Korf solved his famous set of 100 problem instances in 8 minutes and 15 seconds. The framework solved the same code in 10 minutes and 59 seconds.

## 4.6 Events {#s-events}
Each algorithm implementation can define event classes. An event object describes something that happened during the algorithm's execution, such as node selection, node generation etc. The event classes must inherit (possibly indirectly) from \ref slb::core::ui::EventBase. This enables storing the events in the log, analyzing the log in the textual mode and visualizing algorithm's execution. See the [video demo](https://youtu.be/cElxLWve1Zw) for some examples of these capabilities. 

To make sure that no logging-related run-time overhead is incurred in production runs, we provide the \ref slb::core::ui::log function. This function employs *tag dispatch* to choose one of two behaviors *at compile time* based on the compile-time configuration: 
- Do nothing. (In this case, the call is totally eliminated by inlining and no run-time overhead at all is incurred.)
- Construct an event based on the function arguments and add that event into the log. 
The function \ref slb::ext::algorithm::Astar::run provides an example of how events are logged in the existing code.

<!-- Each event defines a visualization effect. It is convenient to derive the events from pre-made bases, which correspond to different kinds of visualization effects. The existing bases are defined in \ref event_types.h and the existing events are defined in \ref events.h. -->

## 4.7 Managed search nodes {#s-nodes}
*Managed search nodes* are yet another convenience that was achieved through the use of macros. A managed search node inherits from \ref slb::core::sb::ManagedNode and is endowed with [reflection capabilities](http://stackoverflow.com/a/11744832/2725810), which the framework uses to define universal output operators. Please refer to \ref node_kinds.h for examples of defining managed search nodes. 

<!-- In particular, this alleviates the need to define an output operator for that node. In addition, the framework is capable to compare the contents of two managed node and produce a string that summarizes the differences. These reflection capabilities come very handy for the textual log analysis tool. -->

## 4.8 Data structures {#s-structures}
The basic data structures are provided by the standard library, the Boost library, as well as many others. These libraries can be freely included and used in the user-provided extensions.

The framework's philosophy towards the data structures directly relevant to implementing the search algorithms is bi-directional. *On the one hand*, the framework aims to provide a small set of such data structures. At present, this set is really minimal. However, the very next research project on the author's agenda is the comparison of the various open list implementations based on the facilities present in the various freely available C++ libraries. This would include both generic implementations and implementations for specific circumstances, such as uniform-cost search. *On the other hand*, the framework leaves full freedom to the researcher to provide his own data structures as extensions. It is also possible that some of these extensions will become part of the core in later versions of the framework.

# 5 Installation {#s-install}
The framework is intended to be used with `Linux`. The instructions in this section have been tested on a fresh standard installation of `Ubuntu 16.04`.

The following actions need to be taken to prepare and run the framework: 
1. Download and extract the framework. Enter the framework's folder.
2. Make sure that the `COMPILER` variable in `Makefile` contains the correct path to version 4.9 or newer of `g++` (that's when `g++` began to fully support regular expressions). 
3. Install `xlib`:

	   sudo apt-get install libx11-dev
4. Install `cairo`:

	   sudo apt-get install libcairo2-dev
5. Make sure that the `CAIRO_PATH` variable in `Makefile` contains the correct path to `cairo.h`.
6. Download and extract the [Boost](http://www.boost.org/users/download/) library. The framework has been tested with version 1.61.0 of Boost.
7. Set the `BOOST_PATH` variable in `Makefile` to point to the `Boost` directory.
8. Install `ncurses`:

	   sudo apt-get install libncurses5-dev

That's it! Now it's time to check the framework out by running the examples from the [video demo](https://youtu.be/cElxLWve1Zw)!
\note Currently keyboard events are caught only when the graphical window is active.

# 6 Other related software {#s-related}
The software frameworks written in `C++` most similar in purpose to this framework are [HOG2](https://github.com/nathansttt/hog2) by Nathan Sturtevant and the [Research Code for Heuristic Search](https://github.com/eaburns/search) by Ethan Burns. There is also the [Combinatorial Search for Java](https://github.com/matthatem/cs4j) written by Matthew Hatem in `Java`. All these frameworks aim for both performance and flexibility. In addition, `HOG2` and the `Research Code for Heuristic Search` are capable of producing visualizations, at least for some domains. However, all three frameworks lack proper documentation, so that one has to understand them from the source code. In addition, my framework provides the following features that are, to the best of my knowledge, not part of any existing framework:
  + Support for analysis of user-provided implementation of search algorithms.
  + Support for visualization of non-grid domains with either user-supplied or automatic layout.
  + Support for compile-time configuration and scripting.
  + The idea of gaining understanding through policy-based design is, to the best of my knowledge, also new in this framework.

Several other projects that are less related to this framework are:
- [OCaml Search Code](https://github.com/jordanthayer/ocaml-search) by Jordan Thayer. This framework is written in `OCaml` and has not been updated since 2012, when the group switched to `C++` for performance reasons.
- [The Boost Graph Library](http://www.boost.org/doc/libs/1_61_0/libs/graph/doc/). `BGL` provides several algorithms for searching in graphs. 
- [PSVN](https://era.library.ualberta.ca/downloads/7m01bn08g) by Robert Holte.
- [Fast Downward](http://www.fast-downward.org/) by Malte Helmert.

# 7 Version and license information {#s-version}
\version 0.1
\author Meir Goldenberg (mgoldenbe@gmail.com)
\copyright MIT License
\warning Boost, Cairo and the Templatized C++ Command Line Parser Library come with their respective licenses. 

# 8 Bug reports and future work {#s-future}
Please send bug reports and feature requests to the author. The current list of requested features and bug reports can be found in the files `future.org` and `bugs.org` in the framework's root folder.
