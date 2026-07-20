CXX ?= clang++
CXXFLAGS ?= -std=c++17 -O3 -march=native -flto
CPPFLAGS ?=
LDFLAGS ?=

TARGET := build/chess_engine
SOURCES := main.cpp uci.cpp bitboard.cpp moves.cpp search.cpp evaluation.cpp utils.cpp types.cpp
HEADERS := uci.hpp bitboard.hpp moves.hpp search.hpp evaluation.hpp utils.hpp types.hpp pieces.hpp

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS) | build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -pthread $(SOURCES) $(LDFLAGS) -o $@

build:
	mkdir -p $@

test: $(TARGET)
	python3 tests/uci_smoke.py $(abspath $(TARGET))

clean:
	rm -f $(TARGET)
