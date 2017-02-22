COMPILER=/usr/bin/g++
CAIRO_PATH=/usr/include/cairo/
BOOST_PATH=~/boost_1_61_0

INCLUDE=-I . -isystem $(CAIRO_PATH) -isystem $(BOOST_PATH) -isystem core/util/outside/

GRAPHICS_LIB=-lcairo -lX11 -lmenu -lncurses

# default values
CPP=test.cpp
MAIN:=$(CPP:.cpp=)
EXEC=$(MAIN)
MODE=debug

MAIN_CPP=$(CPP)
MAIN_I=$(MAIN).i

ifdef CONFIG
	MYCONF=-DCONFIG='"$(CONFIG)"'
endif

# Look at -flto=n in case several translation units are identified
COMMON_PREFIX=$(COMPILER) -Wall -Wextra -Werror -fmax-errors=3 -std=c++11 -pedantic -Wno-sign-compare $(INCLUDE) $(MYCONF) $(ADD)

PREPROCESSOR_DEBUG=$(COMMON_PREFIX) -o $(MAIN_I) $(MAIN_CPP) -E
PREPROCESSOR_PRODUCTION=$(PREPROCESSOR_DEBUG) -DNDEBUG 
MAKE_EXEC=$(COMMON_PREFIX) -fpreprocessed -o $(EXEC) $(MAIN_I) $(GRAPHICS_LIB)
MAKE_DEBUG_EXEC=$(MAKE_EXEC) -g -O0
MAKE_PRODUCTION_EXEC=$(MAKE_EXEC) -O2 #-g -fno-inline

HASH_SOURCE=.execs/last_hash_source
HASH_CODE=.execs/last_hash_code

default: debug

# Builds the executable based on MODE, CPP, CONFIG and ADD.
# Does not use OPT, which controls the run-time parameters.
# Begins by computing the name of executable containing the hash key. Defers the actual build to another instance of make in order to share the EXEC variable.
compute-hash:
	@mkdir -p .execs
	@$(COMMON_PREFIX) -E -fdirectives-only $(MAIN_CPP) -o $(HASH_SOURCE)
	@echo $(MODE) >> $(HASH_SOURCE)
	@cat symbols.cpp >> $(HASH_SOURCE)
	@$(MAKE) 3>&2 2>&1 1>&3 build-exec EXEC=.execs/slb`md5sum $(HASH_SOURCE) | colrm 33 | tee $(HASH_CODE)`

# Both builds and executes.
run: compute-hash
	@./last_exec $(OPT)

# Builds executable with the name computed by compute-hash. Does nothing if the executable exists already.
build-exec:
	@test -e $(EXEC) || $(MAKE) $(MODE)
	@ln -sf $(EXEC) last_exec
	@touch $(EXEC)

preprocessor-debug: symbols
	$(PREPROCESSOR_DEBUG)
	./symbols $(MAIN_I)

preprocessor-production: symbols
	$(PREPROCESSOR_PRODUCTION)
	./symbols $(MAIN_I)

debug: preprocessor-debug
	$(MAKE_DEBUG_EXEC)

production: preprocessor-production
	$(MAKE_PRODUCTION_EXEC)

symbols: symbols.cpp Makefile
	$(COMMON_PREFIX) -O2 symbols.cpp -o symbols

symbols-debug: symbols.cpp Makefile
	$(COMMON_PREFIX) -g symbols.cpp -o symbols

align: align.cpp Makefile
	$(COMMON_PREFIX) -O2 align.cpp -o align

docs:
	rm -rf documentation
	doxygen doxygen.conf
	cd documentation/latex; pdflatex refman.tex; pdflatex refman.tex
	ln -fs documentation/latex/refman.pdf documentation.pdf

#Precompiled header is not currently used
#PRECOMPILED_HEADER=outside_headers.h
#COMMON_PRECOMPILE=$(COMMON_PREFIX) -o $(PRECOMPILED_HEADER).gch -c $(PRECOMPILED_HEADER)

