CXX=clang++
CXXFLAGS=-Wall -Werror -pedantic -std=c++11 -MMD

LLVM_CXXFLAGS=$(shell llvm-config-3.4 --cxxflags)
LLVM_LDFLAGS=$(shell llvm-config-3.4 --ldflags)
LLVM_LIBS=$(shell llvm-config-3.4 --libs all-targets Core Object Jit Interpreter Native)

PWD=$(shell pwd)

PREFIX=$(PWD)
BIN_DIR=$(PREFIX)/bin
X86_64_BIN_DIR=$(BIN_DIR)/x86_64

OBJ_DIR=$(PREFIX)/obj
X86_64_OBJ_DIR=$(OBJ_DIR)/x86_64

SRC_DIR=$(PWD)/src
X86_64_SRC_DIR=$(SRC_DIR)/x86_64

TEST_DIR=$(PWD)/test
X86_64_TEST_DIR=$(TEST_DIR)/x86_64

INC_DIR=$(PWD)/inc

RM=rm -rf
MKDIR=mkdir -p

X86_64_OBJS=                                        \
         $(X86_64_OBJ_DIR)/x86_64_translator.o

X86_64_TEST_OBJS=                                   \
         $(X86_64_OBJ_DIR)/x86_64_translator.test.o

X86_64_TESTS=                                       \
         $(X86_64_BIN_DIR)/x86_64_translator.test

SRC_OBJS=                                           \
         $(OBJ_DIR)/tb.o                            \
         $(OBJ_DIR)/context.o                       \
         $(OBJ_DIR)/translator.o                    \
         $(X86_64_OBJS)

TEST_OBJS=                                          \
         $(OBJ_DIR)/tb.test.o                       \
         $(OBJ_DIR)/context.test.o                  \
         $(X86_64_TEST_OBJS)

TESTS=                                              \
         $(BIN_DIR)/tb.test                         \
         $(BIN_DIR)/context.test                    \
         $(X86_64_TESTS)

default: build_tests

build_tests: make_obj make_bin $(TESTS)

compile_src: make_obj $(SRC_OBJS)

compile_test: make_obj $(TEST_OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -I$(INC_DIR) $(LLVM_CXXFLAGS) $(CXXFLAGS) -c -o $@ $<

$(X86_64_OBJ_DIR)/%.o: $(X86_64_SRC_DIR)/%.cpp
	$(CXX) -I$(INC_DIR) $(LLVM_CXXFLAGS) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.test.o: $(TEST_DIR)/%.cpp
	$(CXX) -I$(INC_DIR) $(LLVM_CXXFLAGS) $(CXXFLAGS) -c -o $@ $<

$(X86_64_OBJ_DIR)/%.test.o: $(X86_64_TEST_DIR)/%.cpp
	$(CXX) -I$(INC_DIR) $(LLVM_CXXFLAGS) $(CXXFLAGS) -c -o $@ $<

$(BIN_DIR)/%.test: $(SRC_OBJS) $(OBJ_DIR)/%.test.o
	$(CXX) $? $(LLVM_LIBS) $(LLVM_LDFLAGS) -s -o $@

$(X86_64_BIN_DIR)/%.test: $(SRC_OBJS) $(X86_64_OBJ_DIR)/%.test.o
	$(CXX) $? $(LLVM_LIBS) $(LLVM_LDFLAGS) -s -o $@

make_obj:
	$(MKDIR) $(OBJ_DIR)
	$(MKDIR) $(X86_64_OBJ_DIR)

make_bin:
	$(MKDIR) $(BIN_DIR)
	$(MKDIR) $(X86_64_BIN_DIR)

clean:
	$(RM) $(OBJ_DIR) $(BIN_DIR)

-include $(OBJ_DIR)/*.d
