#include "board/Board.h"
#include "board/MoveGen.h"
#include "board/Zobrist.h"
#include "engine/Search.h"
#include "engine/Evaluation.h"

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

std::string moveToString(const Move &m)
{
    const char *files = "abcdefgh";
    int fromFile = m.from % 8, fromRank = m.from / 8;
    int toFile = m.to % 8, toRank = m.to / 8;
    std::string s;
    s += files[fromFile];
    s += std::to_string(fromRank + 1);
    s += files[toFile];
    s += std::to_string(toRank + 1);
    return s;
}

void play()
{
    MoveGen::initAttackTables();
    initZobristKeys();

    std::cout << "Simple Chess CLI (You = White)\n";
    std::cout << "Paste a FEN each turn, or type 'exit' to quit.\n\n";

    while (true)
    {
        std::cout << "Enter FEN: ";
        std::string fen;
        std::getline(std::cin, fen);

        if (fen == "q" || fen == "quit")
            break;

        if (fen.empty())
        {
            std::cout << "No FEN entered. Try again.\n";
            continue;
        }

        Board board;
        try
        {
            board.setCustomBoard(fen);
        }
        catch (...)
        {
            std::cout << "Invalid FEN format. Try again.\n";
            continue;
        }
        auto start = std::chrono::high_resolution_clock::now();
        SearchResult res = Search::think(board, 8);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        std::cout << "\nBest move: " << moveToString(res.bestMove)
                  << "\nScore: " << res.score
                  << "\nFound " << res.nodes << " nodes in " << elapsed.count() << " seconds" << "\n\n";
    }
}

void customEval()
{
    Board b;
    b.setCustomBoard("rnbqk1nr/pppp1ppp/8/4p3/1P2P3/8/1P1P1PPP/RNBQKBNR w KQkq - 0 1");
    std::cout << evaluate(b);
    // MoveGen::initAttackTables();
    // std::vector<Move> moves;
    // MoveState state;
    // MoveGen::generatePawnMoves(b, moves);
    // std::cout << moves.size();
    // for (auto m : moves)
    // {
    //     MoveGen::makeMove(b, m, state);
    //     b.printBoard();
    //     std::cout << evaluate(b);
    //     std::cout << "\n";
    //     MoveGen::unmakeMove(b, m, state);
    // }
}

int main()
{
    play();
    // customEval();
    // auto now = std::chrono::system_clock::now();
    // std::time_t startTime = std::chrono::system_clock::to_time_t(now);
    // std::cout << "Started at: " << std::put_time(std::localtime(&startTime), "%Y-%m-%d %H:%M:%S") << "\n";

    // Board b;
    // b.setCustomBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // b.printBoard();
    // initZobristKeys();
    // MoveGen::initAttackTables();
    // for (int depth = 1; depth <= 8; depth++)
    // {
    //     auto start = std::chrono::high_resolution_clock::now();

    //     perftTest(b, depth);

    //     auto end = std::chrono::high_resolution_clock::now();
    //     std::chrono::duration<double> elapsed = end - start;

    //     std::cout << "Depth " << depth << " took "
    //               << elapsed.count() << " seconds" << std::endl;
    // }
}