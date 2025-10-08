all:
	g++ -std=c++17 -Wall -Wextra Board.cpp MoveGen.cpp Zobrist.cpp main.cpp -o c

clean:
	rm -f c chess *.exe
