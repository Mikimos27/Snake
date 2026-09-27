CXX=g++
CXXFLAGS=-g -Wall -Werror -pedantic -O2 -std=c++20 -lncurses
SRCS=game.cpp main.cpp
HDRS=game.hpp
BIN=a.out

all: $(BIN)

a.out: $(SRCS) $(HDRS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS)


.PHONY: clean remake val mem

clean:
	rm -f $(BIN)

remake: clean all

mem: $(SRCS) $(HDRS)
	$(CXX) $(CXXFLAGS) -o $(BIN) $(SRCS) -fsanitize=address

val: $(BIN)
	valgrind --tool=memcheck --leak-check=yes --show-leak-kinds=all --track-origins=yes --log-file=vgr-out.txt -s ./$(BIN)
