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

        if (fen == "q" || fen == "quit" || fen == "exit")
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

        std::cout << (board.whiteToMove ? "White" : "Black") << " to move.\n";

        auto start = std::chrono::high_resolution_clock::now();
        SearchResult res = Search::think(board, 6);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        if (res.bestMove.from == 0 && res.bestMove.to == 0)
        {
            std::cout << "No legal moves found.\n\n";
            continue;
        }

        std::cout << "\nBest move: " << Move::moveToString(res.bestMove)
                  << "\nScore: " << res.score
                  << "\nFound " << res.nodes << " nodes in " << elapsed.count() << " seconds\n\n";
    }
}

void customEval()
{
    Board b;
    b.setCustomBoard("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
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
}