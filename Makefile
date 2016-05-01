# -isystem disables warnings from there
INCLUDE=-isystem ~/boost_1_59_0 -isystem core/util/outside/ -I . -I /usr/include/cairomm-1.0/ -I /usr/include/cairo/ -I /usr/include/sigc++-2.0/ -I /usr/lib/x86_64-linux-gnu/sigc++-2.0/include/ -I /usr/include/freetype2/ -I core/ -I extensions/ 

GRAPHICS_LIB=-lcairo -lX11 -lXtst -lmenu -lncurses

MAIN=test.cpp
PRECOMPILED_HEADER=outside_headers.h

# Look at -flto=n in case several translation units are identified
COMMON_PREFIX=g++ -ftime-report -Wall -Wextra -Werror -fmax-errors=3 -std=c++11 -pedantic $(INCLUDE) -DCONFIG='"$(CONFIG)"'
COMMON=$(COMMON_PREFIX) -H -o Test $(MAIN)
COMMON_PRECOMPILE=$(COMMON_PREFIX) -o $(PRECOMPILED_HEADER).gch -c $(PRECOMPILED_HEADER)
PREPROCESSOR=g++ -Wall -Wextra -Werror -fmax-errors=2 -std=c++11 -pedantic $(INCLUDE) -DCONFIG='"$(CONFIG)"' -E $(MAIN)

BUILD_DEBUG=$(COMMON) -g -DNO_DRAWER
BUILD_DEBUG_DRAWER=$(COMMON) -g -fno-default-inline -fno-inline $(GRAPHICS_LIB)
BUILD_DEBUG_DRAWER_PRECOMPILE=$(COMMON_PRECOMPILE) -g -fno-default-inline -fno-inline $(GRAPHICS_LIB)
BUILD_PRODUCTION=$(COMMON) -DNDEBUG -O2 -DNO_DRAWER 
BUILD_PRODUCTION_DRAWER=$(COMMON) -DNDEBUG -O2 $(GRAPHICS_LIB)

default: debug-drawer

debug:
	$(BUILD_DEBUG)

debug-drawer:
	$(BUILD_DEBUG_DRAWER)

production:
	$(BUILD_PRODUCTION)
	strip Test

production-drawer:
	$(BUILD_PRODUCTION_DRAWER)
	strip Test

preprocessor:
	$(PREPROCESSOR)

precompile:
	$(BUILD_DEBUG_DRAWER_PRECOMPILE)

