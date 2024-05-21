CXX ?= g++

CXXFLAGS := -Wall -std=c++17 -O2

INCLUDES := -I../libs

SRCS := jsonamal.cpp

HEADERS := jsonamal.h ../libs/nlohmann/json.hpp

ifeq ($(OS),Windows_NT)
EXE := .exe
else
EXE :=
endif

.PHONY: all clean

all: jsonamal$(EXE)
	@:

jsonamal$(EXE): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) -o $@ $(LDFLAGS)

clean:
	$(RM) jsonamal jsonamal.exe
