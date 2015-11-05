INCLUDE=-I ~/boost_1_59_0

debug:
	g++ -Wall -Wextra -Werror -std=c++11 -pedantic $(INCLUDE) -g -o Test Test.cpp
production:
	g++ -Wall -Wextra -Werror -std=c++11 -pedantic $(INCLUDE) -o Test Test.cpp
