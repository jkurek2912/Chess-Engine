all:
	g++ -std=c++17 -Wall -Wextra board.cpp main.cpp -o c

clean:
	rm -f c chess *.exe
