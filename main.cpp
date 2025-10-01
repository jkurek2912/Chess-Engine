#include "Board.h"
#include "Movegen.h"
#include <iostream>

// TODO: Threefold repetition (hash table), castling, promotion

uint64_t perft(Board board, int depth, MoveGen &moveGen)
{
    if (depth == 0)
        return 1ULL;

    std::vector<Move> moves;
    moveGen.generateLegalMoves(board, moves);

    uint64_t nodes = 0ULL;
    for (auto &m : moves)
    {
        Board newBoard = board;
        moveGen.applyMove(newBoard, m);
        nodes += perft(newBoard, depth - 1, moveGen);
    }
    return nodes;
}

void perftTest(Board &board, int depth, MoveGen &moveGen)
{
    std::vector<Move> moves;
    moveGen.generateLegalMoves(board, moves);

    uint64_t total = 0ULL;
    for (auto &m : moves)
    {
        Board newBoard = board;
        moveGen.applyMove(newBoard, m);
        uint64_t count = perft(newBoard, depth - 1, moveGen);
        total += count;
    }
    std::cout << "Total nodes at depth " << depth << ": " << total << "\n";
}

int main()
{
    Board b;
    b.setCustomBoard();
    MoveGen gen;
    MoveGen::initAttackTables();
    for (int depth = 1; depth <= 6; depth++)
    {
        perftTest(b, depth, gen);
    }
    return 0;
}