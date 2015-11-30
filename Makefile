# -isystem disables warnings from there
INCLUDE=-isystem ~/boost_1_59_0 -I /usr/include/cairomm-1.0/ -I /usr/include/cairo/ -I /usr/include/sigc++-2.0/ -I /usr/lib/x86_64-linux-gnu/sigc++-2.0/include/ -I /usr/include/freetype2/

GRAPHICS_LIB=-lcairo -lX11

COMMON=g++ -Wall -Wextra -Werror -Wfatal-errors -std=c++11 -pedantic $(INCLUDE) -o Test Test.cpp

BUILD_DEBUG=$(COMMON) -g -DNO_DRAWER
BUILD_DEBUG_DRAWER=$(COMMON) -g $(GRAPHICS_LIB)
BUILD_PRODUCTION=$(COMMON) -DNDEBUG -O2 -DNO_DRAWER
BUILD_PRODUCTION_DRAWER=$(COMMON) -DNDEBUG -O2 $(GRAPHICS_LIB)

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
