CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2 -Isrc -Isrc/board -Isrc/engine
SRC = src/board/Board.cpp src/board/MoveGen.cpp src/board/Zobrist.cpp \
      src/engine/Evaluation.cpp src/engine/Search.cpp src/main.cpp
OBJ = $(SRC:.cpp=.o)
OUT = c

all: clean $(OUT)

$(OUT): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(OUT)

clean:
	rm -f $(OBJ) $(OUT)
