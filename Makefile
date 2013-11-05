CXX=clang++
CXXFLAGS=-Wall -Werror -pedantic -std=c++11 -MMD

LLVM_CXXFLAGS=$(shell llvm-config-3.4 --cxxflags)
LLVM_LDFLAGS=$(shell llvm-config-3.4 --ldflags)
LLVM_LIBS=$(shell llvm-config-3.4 --libs all-targets Core Object)

PWD=$(shell pwd)

PREFIX=$(PWD)
BIN_DIR=$(PREFIX)/bin
OBJ_DIR=$(PREFIX)/obj

SRC_DIR=$(PWD)/src
TEST_DIR=$(PWD)/test
INC_DIR=$(PWD)/inc

RM=rm -rf
MKDIR=mkdir -p

SRC_OBJS=$(OBJ_DIR)/tb.o
TEST_OBJS=$(OBJ_DIR)/tb.test.o
TESTS=$(BIN_DIR)/tb.test

default: build_tests

build_tests: make_obj make_bin $(TESTS)

compile_src: make_obj $(SRC_OBJS)

compile_test: make_obj $(TEST_OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -I$(INC_DIR) $(LLVM_CXXFLAGS) $(CXXFLAGS) -c -o $@ $< 

$(OBJ_DIR)/%.test.o: $(TEST_DIR)/%.cpp
	$(CXX) -I$(INC_DIR) $(LLVM_CXXFLAGS) $(CXXFLAGS) -c -o $@ $<

$(BIN_DIR)/%.test: $(SRC_OBJS) $(OBJ_DIR)/%.test.o
	$(CXX) $? $(LLVM_LIBS) $(LLVM_LDFLAGS) -s -o $@

make_obj:
	$(MKDIR) $(OBJ_DIR)

make_bin:
	$(MKDIR) $(BIN_DIR)

clean:
	$(RM) $(OBJ_DIR) $(BIN_DIR)

-include $(OBJ_DIR)/*.d
