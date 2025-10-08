#include "board/Board.h"
#include "board/MoveGen.h"
#include "board/Zobrist.h"
#include <iostream>
#include <chrono>
#include <future>
#include <thread>
#include <atomic>
#include <iomanip>

uint64_t perft(Board &board, int depth)
{
    if (depth == 0)
        return 1ULL;

    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board, moves);

    uint64_t nodes = 0ULL;
    for (auto &m : moves)
    {
        MoveState state;
        MoveGen::makeMove(board, m, state);
        nodes += perft(board, depth - 1);
        MoveGen::unmakeMove(board, m, state);
    }
    assert((board.occupancy[WHITE] & board.occupancy[BLACK]) == 0);
    assert((board.occupancy[WHITE] | board.occupancy[BLACK]) == board.occupancy[BOTH]);

    // verify en passant square
    if (board.enPassantSquare != -1)
    {
        int rank = board.enPassantSquare / 8;
        assert(rank == 2 || rank == 5 && "EP square should only be on rank 3 (white) or 6 (black)");
    }
    return nodes;
}

void perftTest(Board &board, int depth)
{
    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board, moves);

    const unsigned int maxThreads = 10;
    std::atomic<size_t> nextIndex{0};
    std::atomic<size_t> completed{0};
    std::vector<uint64_t> results(maxThreads, 0);

    size_t totalMoves = moves.size();

    auto worker = [&](int threadId)
    {
        uint64_t subtotal = 0;
        size_t i;
        while ((i = nextIndex.fetch_add(1)) < totalMoves)
        {
            Board localBoard = board;
            Move moveCopy = moves[i];
            MoveState state;

            MoveGen::makeMove(localBoard, moveCopy, state);
            uint64_t count = perft(localBoard, depth - 1);
            subtotal += count;
        }
        results[threadId] = subtotal;
    };

    std::vector<std::thread> threads;
    for (unsigned int t = 0; t < maxThreads; ++t)
        threads.emplace_back(worker, t);

    for (auto &t : threads)
        t.join();

    uint64_t total = 0;
    for (auto n : results)
        total += n;

    std::cout << "\nTotal nodes at depth " << depth << ": " << total << "\n";
}

int main()
{
    auto now = std::chrono::system_clock::now();
    std::time_t startTime = std::chrono::system_clock::to_time_t(now);
    std::cout << "Started at: " << std::put_time(std::localtime(&startTime), "%Y-%m-%d %H:%M:%S") << "\n";

    Board b;
    b.setBoard();
    // initZobristKeys();
    MoveGen::initAttackTables();
    for (int depth = 1; depth <= 7; depth++)
    {
        auto start = std::chrono::high_resolution_clock::now();

        perftTest(b, depth);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        std::cout << "Depth " << depth << " took "
                  << elapsed.count() << " seconds" << std::endl;
    }
}