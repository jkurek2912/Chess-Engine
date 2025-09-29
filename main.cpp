#include "Board.h"
#include "Movegen.h"
#include <iostream>
int main()
{
    Board b;
    b.setBoard();
    b.printBoard();
    b.whiteToMove = false;
    std::cout << "\n";
    b.setCustomBoard();
    b.printBoard();
    std::vector<Move> moves;
    MoveGen::generatePawnMoves(b, moves);
    for (auto move : moves)
    {
        std::cout << move.from << " " << move.to << "\n";
    }
    std::cout << moves.size();
}