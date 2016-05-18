
# -isystem disables warnings from there
INCLUDE=-isystem ~/boost_1_59_0 -isystem core/util/outside/ -I . -I /usr/include/cairomm-1.0/ -I /usr/include/cairo/ -I /usr/include/sigc++-2.0/ -I /usr/lib/x86_64-linux-gnu/sigc++-2.0/include/ -I /usr/include/freetype2/ -I core/ -I extensions/ 

GRAPHICS_LIB=-lcairo -lX11 -lXtst -lmenu -lncurses

# default values
MAIN=test

MAIN_CPP=$(MAIN).cpp
MAIN_I=$(MAIN).i
MAIN_O=$(MAIN).o
PRECOMPILED_HEADER=outside_headers.h

COMPILER=ccache /usr/bin/g++-6

ifdef CONFIG
	MYCONF=-DCONFIG='"$(CONFIG)"'
endif

# Look at -flto=n in case several translation units are identified
COMMON_PREFIX=$(COMPILER) -Wall -Wextra -Werror -fmax-errors=3 -std=c++11 -pedantic -Wno-sign-compare $(INCLUDE) $(MYCONF) $(ADD)
MAKE_OBJ=$(COMMON_PREFIX) $(MAIN_I) -c -fpreprocessed
MAKE_EXEC=$(COMMON_PREFIX) -o $(MAIN) $(MAIN_O)

PREPROCESSOR=$(COMMON_PREFIX) -E $(MAIN_CPP) -o $(MAIN_I) 

MAKE_DEBUG_OBJ=$(MAKE_OBJ) -g -fno-default-inline -fno-inline $(GRAPHICS_LIB)
MAKE_DEBUG_EXEC=$(MAKE_EXEC) -g -fno-default-inline -fno-inline $(GRAPHICS_LIB)

MAKE_PRODUCTION_OBJ=$(MAKE_OBJ) -DNDEBUG -O2 $(GRAPHICS_LIB)
MAKE_PRODUCTION_EXEC=$(MAKE_EXEC) -DNDEBUG -O2 $(GRAPHICS_LIB)

default: debug

pre-raw:
	$(PREPROCESSOR)
	./symbols $(MAIN_I)

debug-raw: pre-raw
	$(MAKE_DEBUG_OBJ)
	$(MAKE_DEBUG_EXEC)

run-debug-raw: debug-raw
	./$(MAIN) $(OPT) >&2

production-raw: pre-raw
	$(MAKE_PRODUCTION_OBJ)
	$(MAKE_PRODUCTION_EXEC)

run-production-raw: production-raw
	./$(MAIN) $(OPT) >&2

run-raw:
	./$(MAIN) $(OPT)

preprocessor-raw:
	$(PREPROCESSOR)

symbols-raw: symbols.cpp Makefile
	$(COMPILER) -Wall -Wextra -Werror -fmax-errors=3 -std=c++11 -pedantic -O2 symbols.cpp -o symbols

%-raw:
	echo "Target $@ does not exist"

%:
	@make $@-raw 3>&2 2>&1 1>&3

# Things not currently used
#COMMON1=$(COMMON_PREFIX) -H -ftime-report -o test $(MAIN)
#COMMON_PRECOMPILE=$(COMMON_PREFIX) -o $(PRECOMPILED_HEADER).gch -c $(PRECOMPILED_HEADER)
#MAKE_DEBUG_PRECOMPILE=$(COMMON_PRECOMPILE) -g -fno-default-inline -fno-inline $(GRAPHICS_LIB)
# precompile:
# 	$(MAKE_DEBUG_DRAWER_PRECOMPILE)
