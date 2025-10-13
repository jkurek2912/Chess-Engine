# === Compiler and Flags ===
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2 -Isrc -Isrc/board -Isrc/engine

OUT = c
DEBUG_OUT = d
TEST_OUT = t

# === GoogleTest Configuration ===
GTEST_DIR = C:/Users/jacob/Desktop/csStuff/googletest
GTEST_INC = $(GTEST_DIR)/googletest/include
GTEST_LIB = $(GTEST_DIR)/build/lib

# === Source Files ===
SRC = src/board/Board.cpp src/board/MoveGen.cpp src/board/Zobrist.cpp \
      src/engine/Evaluation.cpp src/engine/Search.cpp src/engine/Perft.cpp src/main.cpp
OBJ = $(SRC:.cpp=.o)

# === Test Source Files ===
TEST_DIR = src/tests
MOVEGEN_SEARCH_SRCS = $(TEST_DIR)/MoveGenTests.cpp $(TEST_DIR)/SearchTests.cpp $(TEST_DIR)/main_test.cpp \
                      src/board/Board.cpp src/board/MoveGen.cpp src/board/Zobrist.cpp \
                      src/engine/Evaluation.cpp src/engine/Search.cpp
PERFT_SRCS = $(TEST_DIR)/PerftTests.cpp $(TEST_DIR)/main_perft.cpp \
              src/board/Board.cpp src/board/MoveGen.cpp src/board/Zobrist.cpp src/engine/Perft.cpp

.PHONY: all debug clean test perft

# ---------- MAIN BUILD ----------
all: $(OUT)

$(OUT): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(OUT)

# ---------- DEBUG BUILD ----------
debug: CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O0 -g -D_GLIBCXX_DEBUG -Isrc -Isrc/board -Isrc/engine
debug: $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(DEBUG_OUT)

# ---------- MOVEGEN + SEARCH TESTS ----------
test:
	@echo "=== Building MoveGen + Search tests ==="
	$(CXX) $(CXXFLAGS) -DUNIT_TESTING -I$(GTEST_INC) -L$(GTEST_LIB) -o $(TEST_OUT) \
		$(MOVEGEN_SEARCH_SRCS) -lgtest -lgtest_main -pthread
	@echo "=== Running tests ==="
	./$(TEST_OUT)

# ---------- PERFT BENCHMARKS ----------
perft:
	@echo "=== Building Perft benchmark tests ==="
	$(CXX) $(CXXFLAGS) -I$(GTEST_INC) -L$(GTEST_LIB) -o perft_tests \
		$(PERFT_SRCS) -lgtest -pthread
	@echo "=== Running Perft benchmark tests ==="
	./perft_tests

# ---------- CLEAN ----------
clean:
	@echo "=== Cleaning all build artifacts ==="
	rm -f $(OBJ) $(OUT) $(DEBUG_OUT) $(TEST_OUT) perft_tests
