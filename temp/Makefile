INCLUDE=-isystem ~/boost_1_59_0 -isystem . -I /usr/include/cairomm-1.0/ -I /usr/include/cairo/ -I /usr/include/sigc++-2.0/ -I /usr/lib/x86_64-linux-gnu/sigc++-2.0/include/ -I /usr/include/freetype2/

COMPILER=/usr/bin/g++-6

all:
	$(COMPILER) -Wall -Wextra -Werror -std=c++11 -pedantic -Wno-write-strings $(INCLUDE) -g -c temp.cpp -DCONFIG='"$(DEFS)"'
	$(COMPILER) -Wall -Wextra -Werror -std=c++11 -pedantic  -c type.cpp
	$(COMPILER) -Wall -Wextra -Werror -std=c++11 -pedantic -o temp temp.o type.o -lcairo -lX11 -lXtst -lmenu -lform -lncurses

preprocessor:
	$(COMPILER) -Wall -Wextra -Werror -std=c++11 -pedantic $(INCLUDE) -E temp.cpp
