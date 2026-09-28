CXX=g++
CXXFLAGS=-g -Wall -Werror -pedantic -O2 -std=c++20 -lncurses
SRCS=game.cpp main.cpp
HDRS=game.hpp
BIN=snake

all: $(BIN)

$(BIN): $(SRCS) $(HDRS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS)


.PHONY: clean remake open

clean:
	rm -f $(BIN)

remake: clean all


open: $(BIN)
	./$(BIN)
