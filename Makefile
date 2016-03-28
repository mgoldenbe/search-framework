# -isystem disables warnings from there
INCLUDE=-isystem ~/boost_1_59_0 -I /usr/include/cairomm-1.0/ -I /usr/include/cairo/ -I /usr/include/sigc++-2.0/ -I /usr/lib/x86_64-linux-gnu/sigc++-2.0/include/ -I /usr/include/freetype2/

GRAPHICS_LIB=-lcairo -lX11 -lXtst -lmenu -lncurses

# http://stackoverflow.com/q/15280882/2725810
#GRAPHICS_LIB=-DBOOST_SYSTEM_NO_DEPRECATED -lcairo -lX11
#GRAPHICS_LIB=-DBOOST_SYSTEM_NO_DEPRECATED -L~/boost_1_59_0/boost/libs -lboost_system -lcairo -lX11

COMMON=g++ -Wall -Wextra -Werror -fmax-errors=3 -std=c++11 -pedantic $(INCLUDE) -DCONFIG='"$(CONFIG)"' -o Test Test.cpp
PREPROCESSOR=g++ -Wall -Wextra -Werror -fmax-errors=2 -std=c++11 -pedantic $(INCLUDE) -E Test.cpp

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
