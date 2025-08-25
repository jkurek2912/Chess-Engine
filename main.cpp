#include "Board.h"
#include "MoveGen.h"

#include <iostream>
#include <string>

U64 perft(Board b, MoveGen &mg, int depth, COLOR sideToMove)
{
    if (depth == 0)
        return 1ULL;

    U64 nodes = 0;
    std::vector<Move> moves = mg.generateAllLegalMoves(b, sideToMove);

    for (const auto &move : moves)
    {
        Board copy = mg.applyMove(b, move);
        nodes += perft(copy, mg, depth - 1,
                       sideToMove == WHITE ? BLACK : WHITE);
    }

    return nodes;
}

U64 perftDivide(Board b, MoveGen &mg, int depth, COLOR sideToMove)
{
    U64 total = 0;
    auto moves = mg.generateAllLegalMoves(b, sideToMove);

    for (auto &move : moves)
    {
        Board copy = mg.applyMove(b, move);
        U64 nodes = perft(copy, mg, depth - 1,
                          sideToMove == WHITE ? BLACK : WHITE);
        total += nodes;

        std::cout << move.toString() << ": " << nodes << "\n";
    }
    std::cout << "Total: " << total << "\n";
    return total;
}

int main()
{
    Board b;
    b.loadStartPosition();

    MoveGen mg;
    mg.initAttackTables();

    int depth = 2;
    U64 nodes = perftDivide(b, mg, depth, WHITE);
    std::cout << nodes;
}
