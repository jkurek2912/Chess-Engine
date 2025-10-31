#include "board/Board.h"
#include "board/Magic.h"
#include "board/MoveGen.h"
#include "board/Zobrist.h"
#include "engine/Search.h"

#include <iostream>
#include <chrono>

void play()
{
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
        board.setCustomBoard(fen);

        std::cout << (board.whiteToMove ? "White" : "Black") << " to move.\n";

        auto start = std::chrono::high_resolution_clock::now();
        SearchResult res = Search::think(board);
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

int main()
{
    MoveGen::initAttackTables();
    initZobristKeys();
    initMagicBitboards();
    play();
}