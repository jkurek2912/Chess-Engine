CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2 -Isrc -Isrc/board -Isrc/engine

OUT = c
DEBUG_OUT = d
TEST_OUT = t

# GoogleTest config
GTEST_DIR = C:/Users/jacob/Desktop/csStuff/googletest
GTEST_INC = $(GTEST_DIR)/googletest/include
GTEST_LIB = $(GTEST_DIR)/build/lib

# Source files
SRC = src/board/Board.cpp src/board/MoveGen.cpp src/board/Zobrist.cpp \
      src/engine/Evaluation.cpp src/engine/Search.cpp src/engine/Perft.cpp src/main.cpp
OBJ = $(SRC:.cpp=.o)

# Test sources
TEST_DIR = src/tests
TEST_SRCS = $(wildcard $(TEST_DIR)/*.cpp) $(filter-out src/main.cpp,$(SRC))

.PHONY: all clean

# ---------- MAIN ----------
all: $(OUT)

$(OUT): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(OUT)

# ---------- TEST BUILD ----------
$(TEST_OUT): CXXFLAGS := $(CXXFLAGS) -DUNIT_TESTING -I$(GTEST_INC)
$(TEST_OUT): $(TEST_SRCS)
	$(CXX) $(CXXFLAGS) -L$(GTEST_LIB) -o $(TEST_OUT) $^ -lgtest -lgtest_main -pthread
	@echo "=== Running tests ==="
	./$(TEST_OUT)

# ---------- DEBUG BUILD ----------
$(DEBUG_OUT): CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O0 -g -D_GLIBCXX_DEBUG -Isrc -Isrc/board -Isrc/engine
$(DEBUG_OUT): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o d.exe
	mv d.exe $(DEBUG_OUT)

# ---------- CLEAN ----------
clean:
	@echo "=== Cleaning all build artifacts ==="
	rm -f $(OBJ) $(OUT) $(DEBUG_OUT) $(TEST_OUT) d.exe
