INCLUDE=-I ~/boost_1_59_0 -I /usr/include/cairomm-1.0/ -I /usr/include/cairo/ -I /usr/include/sigc++-2.0/ -I /usr/lib/x86_64-linux-gnu/sigc++-2.0/include/ -I /usr/include/freetype2/

all:
	g++ -Wall -Wextra -Werror -std=c++11 -pedantic $(INCLUDE) -g -c temp.cpp
	g++ -Wall -Wextra -Werror -std=c++11 -pedantic  -c type.cpp
	g++ -Wall -Wextra -Werror -std=c++11 -pedantic  -o temp temp.o type.o -lcairo -lX11
