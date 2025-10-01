#include "Board.h"
#include "Movegen.h"
#include <iostream>
#include <chrono>

// TODO: Optimization: make / unmake moves to avoid copying board for every move

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
    for (int depth = 1; depth <= 7; depth++)
    {
        auto start = std::chrono::high_resolution_clock::now();

        perftTest(b, depth, gen);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        std::cout << "Depth " << depth << " took "
                  << elapsed.count() << " seconds" << std::endl;
    }
    return 0;
}