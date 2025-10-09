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

    // Generate all legal root moves
    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board, moves);

    if (moves.empty())
    {
        if (MoveGen::inCheck(board, board.whiteToMove ? BLACK : WHITE))
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
        int score = negamax(localBoard, depth - 1, -INF, INF, localNodes, dummy);

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

int Search::negamax(Board &board, int depth, int alpha, int beta, uint64_t &nodes, Move &bestMoveOut)
{
    nodes++;

    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board, moves);

    if (moves.empty())
    {
        if (MoveGen::inCheck(board, board.whiteToMove ? BLACK : WHITE))
        {
            return -(MATE_SCORE - depth); // checkmated
        }
        else
        {
            return 0; // stalemate
        }
    }

    if (depth == 0)
        if (depth == 0)
            return board.whiteToMove ? evaluate(board) : -evaluate(board);

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
