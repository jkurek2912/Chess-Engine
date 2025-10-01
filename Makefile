all:
	g++ -std=c++17 -Wall -Wextra board.cpp MoveGen.cpp main.cpp -o c

clean:
	rm -f c chess *.exe
