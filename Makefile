# -isystem disables warnings from there
INCLUDE=-isystem ~/boost_1_59_0 -isystem core/util/outside/ -I /usr/include/cairomm-1.0/ -I /usr/include/cairo/ -I /usr/include/sigc++-2.0/ -I /usr/lib/x86_64-linux-gnu/sigc++-2.0/include/ -I /usr/include/freetype2/ -I core/ -I extensions/ 

GRAPHICS_LIB=-lcairo -lX11 -lXtst -lmenu -lncurses

MAIN=Test.cpp

COMMON=g++ -Wall -Wextra -Werror -fmax-errors=3 -std=c++11 -pedantic $(INCLUDE) -DCONFIG='"$(CONFIG)"' -o Test $(MAIN)
PREPROCESSOR=g++ -Wall -Wextra -Werror -fmax-errors=2 -std=c++11 -pedantic $(INCLUDE) -DCONFIG='"$(CONFIG)"' -E $(MAIN)

BUILD_DEBUG=$(COMMON) -g -DNO_DRAWER
BUILD_DEBUG_DRAWER=$(COMMON) -g -fno-default-inline -fno-inline $(GRAPHICS_LIB)
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
