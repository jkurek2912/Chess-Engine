#include "Perft.h"
#include "MoveGen.h"

#include <iostream>
#include <chrono>
#include <future>
#include <thread>
#include <atomic>
#include <iomanip>
#include <cassert>

// Recursive single-threaded perft
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
    return nodes;
}

// Top-level perftTest with optional multithreading
// threads = 1  -> single-threaded
// threads > 1 -> multi-threaded
uint64_t perftTest(Board &board, int depth, unsigned int threads)
{
    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board, moves);

    if (threads <= 1)
    {
        uint64_t total = 0;
        for (auto &m : moves)
        {
            MoveState st;
            MoveGen::makeMove(board, m, st);
            total += perft(board, depth - 1);
            MoveGen::unmakeMove(board, m, st);
        }
        return total;
    }

    const unsigned int maxThreads = std::min<unsigned int>(threads, moves.size());
    std::atomic<size_t> nextIndex{0};
    std::vector<uint64_t> results(maxThreads, 0);

    auto worker = [&](unsigned int threadId)
    {
        uint64_t subtotal = 0;
        size_t i;
        while ((i = nextIndex.fetch_add(1)) < moves.size())
        {
            Board localBoard = board;
            MoveState st;
            Move moveCopy = moves[i];
            MoveGen::makeMove(localBoard, moveCopy, st);
            subtotal += perft(localBoard, depth - 1);
        }
        results[threadId] = subtotal;
    };

    std::vector<std::thread> pool;
    pool.reserve(maxThreads);
    for (unsigned int t = 0; t < maxThreads; ++t)
        pool.emplace_back(worker, t);

    for (auto &t : pool)
        t.join();

    uint64_t total = 0;
    for (auto n : results)
        total += n;

    return total;
}
