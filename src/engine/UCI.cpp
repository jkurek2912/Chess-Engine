#include "UCI.h"
#include "board/Magic.h"
#include "board/Zobrist.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <thread>

Board UCI::currentBoard;
bool UCI::isInitialized = false;

std::vector<std::string> UCI::split(const std::string &s)
{
    std::vector<std::string> tokens;
    std::istringstream iss(s);
    std::string token;
    while (iss >> token)
    {
        tokens.push_back(token);
    }
    return tokens;
}

void UCI::run()
{
    std::string line;

    MoveGen::initAttackTables();
    initZobristKeys();
    initMagicBitboards();
    isInitialized = true;

    while (std::getline(std::cin, line))
    {
        if (line.empty())
            continue;

        processCommand(line);
    }
}

void UCI::processCommand(const std::string &command)
{
    std::vector<std::string> tokens = split(command);
    if (tokens.empty())
        return;

    std::string cmd = tokens[0];

    if (cmd == "uci")
    {
        uci();
    }
    else if (cmd == "debug")
    {
        debug(tokens.size() > 1 && tokens[1] == "on");
    }
    else if (cmd == "isready")
    {
        isready();
    }
    else if (cmd == "setoption")
    {
        setoption(command);
    }
    else if (cmd == "ucinewgame")
    {
        ucinewgame();
    }
    else if (cmd == "position")
    {
        std::string fen;
        std::vector<std::string> moves;
        bool foundMoves = false;

        for (size_t i = 1; i < tokens.size(); i++)
        {
            if (tokens[i] == "moves")
            {
                foundMoves = true;
                continue;
            }

            if (foundMoves)
            {
                moves.push_back(tokens[i]);
            }
            else if (tokens[i] != "fen" && tokens[i] != "startpos")
            {
                if (tokens[i] == "startpos")
                {
                    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
                }
                else
                {
                    fen += tokens[i];
                    if (i < tokens.size() - 1 && tokens[i + 1] != "moves")
                        fen += " ";
                }
            }
        }

        if (tokens.size() > 1 && tokens[1] == "startpos")
        {
            fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        }

        position(fen, moves);
    }
    else if (cmd == "go")
    {
        go(command);
    }
    else if (cmd == "stop")
    {
        stop();
    }
    else if (cmd == "quit")
    {
        quit();
    }
}

void UCI::uci()
{
    std::cout << "id name Chess Engine" << std::endl;
    std::cout << "id author Your Name" << std::endl;

    // Report UCI options (required by lichess-bot)
    std::cout << "option name Threads type spin default 1 min 1 max 1" << std::endl;
    std::cout << "option name Hash type spin default 64 min 1 max 1024" << std::endl;
    std::cout << "option name Clear Hash type button" << std::endl;
    std::cout << "option name Ponder type check default false" << std::endl;
    std::cout << "option name MultiPV type spin default 1 min 1 max 1" << std::endl;
    std::cout << "option name Skill Level type spin default 20 min 0 max 20" << std::endl;
    std::cout << "option name Move Overhead type spin default 30 min 0 max 5000" << std::endl;
    std::cout << "option name Slow Mover type spin default 89 min 10 max 1000" << std::endl;
    std::cout << "option name nodestime type spin default 0 min 0 max 10000" << std::endl;
    std::cout << "option name UCI_Chess960 type check default false" << std::endl;

    std::cout << "uciok" << std::endl;
}

void UCI::debug(bool on)
{
    // Debug mode - not implemented yet
}

void UCI::isready()
{
    std::cout << "readyok" << std::endl;
}

void UCI::setoption(const std::string &option)
{
    std::vector<std::string> tokens = split(option);

    // Parse "setoption name <name> [value <value>]"
    // Option name can be multi-word, like "Clear Hash" or "Skill Level"
    if (tokens.size() < 3 || tokens[0] != "setoption" || tokens[1] != "name")
    {
        return; // Invalid format, ignore
    }

    // Find where "value" keyword starts (if present)
    size_t valueIdx = tokens.size();
    for (size_t i = 2; i < tokens.size(); i++)
    {
        if (tokens[i] == "value")
        {
            valueIdx = i;
            break;
        }
    }

    // Build option name (can be multi-word)
    std::string optionName;
    for (size_t i = 2; i < valueIdx; i++)
    {
        if (i > 2)
            optionName += " ";
        optionName += tokens[i];
    }

    std::string optionValue;
    if (valueIdx < tokens.size() && valueIdx + 1 < tokens.size())
    {
        optionValue = tokens[valueIdx + 1];
    }
}

void UCI::ucinewgame()
{
    currentBoard.setBoard();
    currentBoard.hash = currentBoard.computeZobrist();
    currentBoard.repetitionCount.clear();
    currentBoard.repetitionCount[currentBoard.hash] = 1;
}

void UCI::position(const std::string &fen, const std::vector<std::string> &moves)
{
    try
    {
        if (fen.empty())
        {
            currentBoard.setBoard();
        }
        else
        {
            currentBoard.setCustomBoard(fen);
        }

        currentBoard.hash = currentBoard.computeZobrist();
        currentBoard.repetitionCount.clear();
        currentBoard.repetitionCount[currentBoard.hash] = 1;

        // Apply moves from the moves list
        // Each move should match the current side to move, and after applying it,
        // the side flips (handled by makeMove)
        for (const auto &moveStr : moves)
        {
            try
            {
                // Get the expected color BEFORE checking the move
                Color expectedColor = currentBoard.whiteToMove ? WHITE : BLACK;

                Move move = Move::fromUCIString(moveStr, currentBoard);

                // Only apply moves that match the current side to move
                if (move.color != expectedColor)
                {
                    // This move doesn't match the current side - skip it
                    // This prevents applying moves that would flip sides incorrectly
                    continue;
                }

                std::vector<Move> legalMoves;
                MoveGen::generateLegalMoves(currentBoard, legalMoves);

                bool found = false;
                for (const auto &legalMove : legalMoves)
                {
                    if (legalMove.from == move.from && legalMove.to == move.to)
                    {
                        if (move.isPromotion && legalMove.isPromotion)
                        {
                            if (legalMove.piece == move.piece)
                            {
                                move = legalMove;
                                found = true;
                                break;
                            }
                        }
                        else if (!move.isPromotion && !legalMove.isPromotion)
                        {
                            move = legalMove;
                            found = true;
                            break;
                        }
                    }
                }

                if (!found)
                {
                    // Move is not legal - don't apply it
                    continue;
                }

                // Apply the move - makeMove will flip whiteToMove
                MoveState state;
                MoveGen::makeMove(currentBoard, move, state);
            }
            catch (const std::exception &e)
            {
                // If move parsing fails, skip it
            }
        }
    }
    catch (const std::exception &e)
    {
        // Position setting failed - this will be caught by lichess-bot
    }
}

int UCI::parseDepth(const std::string &params)
{
    std::vector<std::string> tokens = split(params);
    for (size_t i = 0; i < tokens.size(); i++)
    {
        if (tokens[i] == "depth" && i + 1 < tokens.size())
        {
            return std::stoi(tokens[i + 1]);
        }
    }
    return 0;
}

int UCI::parseTimeLimit(const std::string &params, bool isWhite)
{
    std::vector<std::string> tokens = split(params);
    for (size_t i = 0; i < tokens.size(); i++)
    {
        std::string token = tokens[i];
        if (isWhite && token == "wtime" && i + 1 < tokens.size())
        {
            return std::stoi(tokens[i + 1]);
        }
        else if (!isWhite && token == "btime" && i + 1 < tokens.size())
        {
            return std::stoi(tokens[i + 1]);
        }
    }
    return 0;
}

int UCI::parseMoveTime(const std::string &params)
{
    std::vector<std::string> tokens = split(params);
    for (size_t i = 0; i < tokens.size(); i++)
    {
        if (tokens[i] == "movetime" && i + 1 < tokens.size())
        {
            return std::stoi(tokens[i + 1]);
        }
    }
    return 0;
}

void UCI::go(const std::string &params)
{
    int depth = parseDepth(params);
    int moveTime = parseMoveTime(params);
    int timeLimit = parseTimeLimit(params, currentBoard.whiteToMove);

    int maxDepth = depth > 0 ? depth : 10; // Default depth if not specified

    Color expectedColor = currentBoard.whiteToMove ? WHITE : BLACK;

    auto start = std::chrono::steady_clock::now();
    SearchResult result = Search::think(currentBoard, maxDepth);
    auto end = std::chrono::steady_clock::now();

    // Get all legal moves for validation
    std::vector<Move> legalMoves;
    MoveGen::generateLegalMoves(currentBoard, legalMoves);

    // Ensure the best move is for the correct side
    if (result.bestMove.from != 0 || result.bestMove.to != 0)
    {
        if (result.bestMove.color != expectedColor)
        {
            // Find first legal move with correct color
            for (const auto &m : legalMoves)
            {
                if (m.color == expectedColor)
                {
                    result.bestMove = m;
                    break;
                }
            }
        }

        std::string moveStr = Move::moveToString(result.bestMove);
        std::cout << "bestmove " << moveStr << std::endl;
    }
    else
    {
        std::cout << "bestmove 0000" << std::endl; // Null move if no legal moves
    }
}

void UCI::stop()
{
    // Stop searching - not fully implemented (would need threading)
}

void UCI::quit()
{
    exit(0);
}
