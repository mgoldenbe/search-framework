INCLUDE=-I ~/boost_1_59_0

all:
	g++ -Wall -Wextra -Werror -std=c++11 -pedantic $(INCLUDE) -c temp.cpp
	g++ -Wall -Wextra -Werror -std=c++11 -pedantic  -c type.cpp
	g++ -Wall -Wextra -Werror -std=c++11 -pedantic  -o temp temp.o type.o
