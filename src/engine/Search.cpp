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
    MoveGen::generateLegalMoves(board);
    orderMoves(board.legalMoves);

    if (board.legalMoves.empty())
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

int qsearch(Board &board, int alpha, int beta, uint64_t &nodes)
{
    nodes++;

    // Evaluate current static position
    int stand = evaluate(board);
    if (stand >= beta)
        return beta;
    if (stand > alpha)
        alpha = stand;

    // Generate all moves normally
    MoveGen::generateLegalMoves(board);

    // Only consider captures (and optionally promotions)
    for (auto &m : board.legalMoves)
    {
        if (!m.isCapture && !m.isPromotion)
            continue;

        MoveState st;
        MoveGen::makeMove(board, m, st);
        int score = -qsearch(board, -beta, -alpha, nodes);
        MoveGen::unmakeMove(board, m, st);

        if (score >= beta)
            return beta;
        if (score > alpha)
            alpha = score;
    }

    return alpha;
}

int Search::negamax(Board &board, int depth, int alpha, int beta, uint64_t &nodes, Move &bestMoveOut, int ply)
{
    nodes++;

    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board);

    if (board.legalMoves.empty())
    {
        const bool stmInCheck = MoveGen::inCheck(board, board.whiteToMove ? WHITE : BLACK);
        if (stmInCheck)
        {
            return board.whiteToMove ? (-MATE_SCORE + ply) : (+MATE_SCORE - ply);
        }
        else
        {
            return 0; // stalemate
        }
    }
    orderMoves(board.legalMoves);

    if (depth == 0)
    {
        return qsearch(board, alpha, beta, nodes);
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