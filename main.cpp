#include "Board.h"
#include "Movegen.h"
#include <iostream>
int main()
{
    Board b;
    MoveGen::initAttackTables();
    b.setCustomBoard();
    b.printBoard();
    b.whiteToMove = false;
    std::vector<Move> moves;
    MoveGen::generatePawnMoves(b, moves);
    for (auto move : moves)
    {
        std::cout << move.from << " " << move.to << "\n";
    }
    std::cout << moves.size() << "\n";
    for (auto move : moves)
    {
        Board copy = b;
        MoveGen::applyMove(copy, move);
        copy.printBoard();
        std::cout << "\n";
    }
}