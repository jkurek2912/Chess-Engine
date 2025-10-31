#include "board/Board.h"
#include "board/Magic.h"
#include "board/MoveGen.h"
#include "board/Zobrist.h"
#include "engine/Search.h"

#include <iostream>
#include <chrono>
#include <stdexcept>

static constexpr const char *EXIT_COMMANDS[] = {"q", "quit", "exit"};

bool isExitCommand(const std::string &input)
{
    for (const auto *cmd : EXIT_COMMANDS)
    {
        if (input == cmd)
            return true;
    }
    return false;
}

bool isNullMove(const Move &move)
{
    return move.from == 0 && move.to == 0 && move.piece == PAWN;
}

void evaluateFenPosition()
{
    std::cout << "Simple Chess CLI\n";
    std::cout << "Paste a FEN each turn, or type 'q'/'quit'/'exit' to quit.\n\n";

    while (true)
    {
        std::cout << "Enter FEN: ";
        std::string fen;

        if (!std::getline(std::cin, fen))
        {
            if (std::cin.eof())
            {
                std::cout << "\nEOF detected. Exiting.\n";
                break;
            }
            std::cout << "Input error occurred. Exiting.\n";
            break;
        }

        if (isExitCommand(fen))
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
        catch (const std::invalid_argument &e)
        {
            std::cout << "Invalid FEN: " << e.what() << "\nTry again.\n\n";
            continue;
        }
        catch (const std::exception &e)
        {
            std::cout << "Error parsing FEN: " << e.what() << "\nTry again.\n\n";
            continue;
        }

        std::cout << (board.whiteToMove ? "White" : "Black") << " to move.\n";

        std::vector<Move> legalMoves;
        MoveGen::generateLegalMoves(board, legalMoves);

        if (legalMoves.empty())
        {
            bool inCheck = MoveGen::inCheck(board, board.whiteToMove ? WHITE : BLACK);
            if (inCheck)
            {
                std::string winner = board.whiteToMove ? "Black" : "White";
                std::cout << "\n"
                          << (board.whiteToMove ? "White" : "Black") << " is in checkmate!\n";
                std::cout << winner << " wins!\n";
                std::cout << "Game Over.\n\n";
                break;
            }
            else
            {
                std::cout << "\nStalemate! It's a draw.\n";
                std::cout << "Game Over.\n\n";
                break;
            }
        }

        auto start = std::chrono::steady_clock::now();
        SearchResult res = Search::think(board);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        if (isNullMove(res.bestMove))
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
    try
    {
        MoveGen::initAttackTables();
        initZobristKeys();
        initMagicBitboards();
        evaluateFenPosition();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Fatal error during initialization: " << e.what() << "\n";
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown fatal error during initialization\n";
        return 1;
    }
    return 0;
}