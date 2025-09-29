#include "Board.h"
#include "Movegen.h"
#include <iostream>

// TODO: Threefold repetition, hash table, bishop rook moves -> queen moves, then king moves
int main()
{
    Board b;
    MoveGen::initAttackTables();
    b.setCustomBoard();
    b.printBoard();
    std::vector<Move> moves;
    MoveGen::generateLegalMoves(b, moves);
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