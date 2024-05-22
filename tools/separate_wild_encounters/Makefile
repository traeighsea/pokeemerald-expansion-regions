CXX ?= g++

CXXFLAGS := -Wall -std=c++17 -O2 -g

INCLUDES := -I../libs

SRCS := separate_wild_encounters.cpp

HEADERS := separate_wild_encounters.h ../libs/nlohmann/json.hpp

ifeq ($(OS),Windows_NT)
EXE := .exe
else
EXE :=
endif

.PHONY: all clean

all: separate_wild_encounters$(EXE)
	@:

separate_wild_encounters$(EXE): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) -o $@ $(LDFLAGS)

clean:
	$(RM) separate_wild_encounters separate_wild_encounters.exe
