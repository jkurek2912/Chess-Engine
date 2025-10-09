#include "Search.h"
#include "MoveGen.h"
#include <algorithm>
#include <future>
#include <limits>
#include <iostream>

static constexpr int MATE_SCORE = 1000000;
static constexpr int INF = 10000000;

void orderMoves(std::vector<Move> &moves)
{
    std::partition(moves.begin(), moves.end(),
                   [](const Move &m)
                   { return m.isCapture; });
}

SearchResult Search::think(Board &board, int depth)
{
    SearchResult result{};
    result.nodes = 0;

    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board, moves);
    orderMoves(moves);

    if (moves.empty())
    {
        if (MoveGen::inCheck(board, board.whiteToMove ? WHITE : BLACK))
            result.score = -MATE_SCORE;
        else
            result.score = 0;
        return result;
    }

    int bestScore = -INF;
    Move bestMove{};
    uint64_t totalNodes = 0;

    for (auto &m : moves)
    {
        Board localBoard = board;
        MoveState st;
        MoveGen::makeMove(localBoard, m, st);

        uint64_t localNodes = 0;
        Move dummy;
        int score = -negamax(localBoard, depth - 1, -INF, INF, localNodes, dummy, 1);

        MoveGen::unmakeMove(localBoard, m, st);

        totalNodes += localNodes;

        if (score > bestScore)
        {
            bestScore = score;
            bestMove = m;
        }
    }

    result.bestMove = bestMove;
    result.score = bestScore;
    result.nodes = totalNodes;
    return result;
}

int Search::negamax(Board &board, int depth, int alpha, int beta, uint64_t &nodes, Move &bestMoveOut, int ply)
{
    nodes++;

    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board, moves);
    orderMoves(moves);

    if (moves.empty())
    {
        if (MoveGen::inCheck(board, board.whiteToMove ? WHITE : BLACK))
        {
            return -(MATE_SCORE - ply); // checkmated
        }
        else
        {
            return 0; // stalemate
        }
    }

    if (depth == 0)
    {
        auto eval = evaluate(board);
        if (eval.second)
            return negamax(board, 2, alpha, beta, nodes, bestMoveOut, ply);

        return (board.whiteToMove ? 1 : -1) * eval.first;
    }

    int best = -INF;
    Move bestMove;

    for (auto &m : moves)
    {
        MoveState st;
        MoveGen::makeMove(board, m, st);

        Move childBest;
        int score = -negamax(board, depth - 1, -beta, -alpha, nodes, childBest, ply + 1);

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