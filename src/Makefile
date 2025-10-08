CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2 -I. -Iboard -Iengine
SRC = board/Board.cpp board/MoveGen.cpp board/Zobrist.cpp \
      engine/Evaluation.cpp engine/Search.cpp main.cpp
OBJ = $(SRC:.cpp=.o)
OUT = c

all: $(OUT)

$(OUT): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(OUT)

clean:
	rm -f $(OBJ) $(OUT)
