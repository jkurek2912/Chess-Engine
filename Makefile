CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2 -Isrc -Isrc/board -Isrc/engine

OUT = c
TEST_OUT = test

SRC = src/board/Board.cpp src/board/MoveGen.cpp src/board/Zobrist.cpp \
      src/engine/Evaluation.cpp src/engine/Search.cpp src/engine/Perft.cpp src/main.cpp
OBJ = $(SRC:.cpp=.o)

GTEST_DIR = C:/Users/jacob/Desktop/csStuff/googletest
GTEST_INC = $(GTEST_DIR)/googletest/include
GTEST_LIB = $(GTEST_DIR)/build/lib
TEST_DIR = src/tests
TEST_SRCS = $(wildcard $(TEST_DIR)/*.cpp) $(filter-out src/main.cpp,$(SRC))

all: $(OUT) $(TEST_OUT)

$(OUT): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(OUT)

$(TEST_OUT): $(TEST_SRCS)
	$(CXX) $(CXXFLAGS) -I$(GTEST_INC) -L$(GTEST_LIB) -o $(TEST_OUT) $^ -lgtest -lgtest_main -pthread

clean:
	rm -f $(OBJ) $(OUT) $(TEST_OUT)
