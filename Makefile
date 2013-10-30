CXX=clang++
CXXFLAGS=-Wall -Werror -pedantic -std=c++11

LLVM_CXXFLAGS=$(shell llvm-config-3.4 --cxxflags)
LLVM_LDFLAGS=$(shell llvm-config-3.4 --ldflags)
LIBS=$(shell llvm-config-3.4 --libs all-targets Core Object)

PWD=$(shell pwd)

PREFIX=$(PWD)
BIN_DIR=$(PREFIX)/bin

SRC_DIR=$(PWD)/src
TEST_DIR=$(PWD)/test
INC_DIR=$(PWD)/inc

RM=rm -rf
MKDIR=mkdir -p

OBJS=$(SRC_DIR)/tb.o

default: build_src

build_src: $(OBJS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -I$(INC_DIR) $(LLVM_CXXFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(SRC_DIR)/*.o
