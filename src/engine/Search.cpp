#include "Search.h"
#include "MoveGen.h"
#include <future>
#include <limits>
#include <iostream>

static constexpr int MATE_SCORE = 1000000;
static constexpr int INF = 10000000;

SearchResult Search::think(Board &board, int depth)
{
    SearchResult result{};
    result.nodes = 0;

    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board, moves);
    if (moves.empty())
    {
        if (MoveGen::inCheck(board, board.whiteToMove ? WHITE : BLACK))
            result.score = -MATE_SCORE + board.moves;
        else
            result.score = 0;
        return result;
    }

    struct JobResult
    {
        int score;
        Move move;
        uint64_t nodes;
    };

    unsigned maxThreads = 8;
    unsigned idx = 0;
    int bestScore = -INF;
    Move bestMove{};
    uint64_t totalNodes = 0;

    while (idx < moves.size())
    {
        std::vector<std::future<JobResult>> futures;
        for (unsigned t = 0; t < maxThreads && idx < moves.size(); ++t, ++idx)
        {
            Move m = moves[idx];
            futures.push_back(std::async(std::launch::async, [board, m, depth]() mutable
                                         {
                Board localBoard = board;
                Move localMove = m;
                MoveState st;

                MoveGen::makeMove(localBoard, localMove, st);
                uint64_t localNodes = 0;
                Move dummy;
                int score = -negamax(localBoard, depth - 1, -INF, INF, localNodes, dummy);
                MoveGen::unmakeMove(localBoard, localMove, st);

                return JobResult{score, localMove, localNodes}; }));
        }

        for (auto &f : futures)
        {
            auto r = f.get();
            if (r.score > bestScore)
            {
                bestScore = r.score;
                bestMove = r.move;
            }
            totalNodes += r.nodes;
        }
    }

    result.bestMove = bestMove;
    result.score = bestScore;
    result.nodes = totalNodes;
    return result;
}

int Search::negamax(Board &board, int depth, int alpha, int beta, uint64_t &nodes, Move &bestMoveOut)
{
    nodes++;

    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board, moves);
    if (moves.empty())
    {
        if (MoveGen::inCheck(board, board.whiteToMove ? WHITE : BLACK))
        {
            return -MATE_SCORE + board.moves; // checkmated
        }
        else
        {
            return 0; // stalemate
        }
    }

    if (depth == 0)
        return evaluate(board);

    int best = -INF;
    Move bestMove;

    for (auto &m : moves)
    {
        MoveState st;
        MoveGen::makeMove(board, m, st);

        Move childBest;
        int score = -negamax(board, depth - 1, -beta, -alpha, nodes, childBest);

        MoveGen::unmakeMove(board, m, st);

        if (score > best)
        {
            best = score;
            bestMove = m;
        }

        alpha = std::max(alpha, best);
        if (alpha >= beta)
            break; // pruning okay below root
    }

    bestMoveOut = bestMove;
    return best;
}