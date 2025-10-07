#include "Board.h"
#include "Movegen.h"
#include <iostream>
#include <chrono>
#include <iomanip>

uint64_t perft(Board &board, int depth, MoveGen &moveGen)
{
    if (depth == 0)
        return 1ULL;

    std::vector<Move> moves;
    moveGen.generateLegalMoves(board, moves);

    uint64_t nodes = 0ULL;
    for (auto &m : moves)
    {
        MoveState state;
        MoveGen::makeMove(board, m, state);
        nodes += perft(board, depth - 1, moveGen);
        MoveGen::unmakeMove(board, m, state);
    }
    return nodes;
}

void perftTest(Board &board, int depth, MoveGen &moveGen)
{
    std::vector<Move> moves;
    moveGen.generateLegalMoves(board, moves);

    uint64_t total = 0ULL;
    size_t totalMoves = moves.size();

    for (size_t i = 0; i < totalMoves; ++i)
    {
        auto &m = moves[i];
        MoveState state;
        MoveGen::makeMove(board, m, state);
        uint64_t count = perft(board, depth - 1, moveGen);
        MoveGen::unmakeMove(board, m, state);
        total += count;

        double progress = double(i + 1) / totalMoves * 100.0;
        std::cout << "\rProgress: " << std::fixed << std::setprecision(1)
                  << progress << "% (" << (i + 1) << "/" << totalMoves << ")"
                  << std::flush;
    }

    std::cout << "\nTotal nodes at depth " << depth << ": " << total << "\n";
}

int main()
{
    Board b;
    b.setBoard();
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