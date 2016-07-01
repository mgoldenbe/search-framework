INCLUDE=. -isystem ~/boost_1_61_0 -isystem -I /usr/include/cairo/

COMPILER=/usr/bin/g++

all:
	$(COMPILER) -Wall -Wextra -Werror -std=c++11 -pedantic -Wno-write-strings $(INCLUDE) -g -O0 -c temp.cpp -DCONFIG='"$(DEFS)"'
#	$(COMPILER) -Wall -Wextra -Werror -std=c++11 -pedantic  -c type.cpp
	$(COMPILER) -Wall -Wextra -Werror -std=c++11 -pedantic -o temp temp.o type.o -lcairo -lX11 -lmenu -lform -lncurses

preprocessor:
	$(COMPILER) -Wall -Wextra -Werror -std=c++11 -pedantic $(INCLUDE) -E temp.cpp
