#include "Search.h"
#include <limits>

static constexpr int INF = std::numeric_limits<int>::max() / 4;

SearchResult Search::think(Board &board, int depth)
{
    SearchResult result{};
    result.nodes = 0;

    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board, moves);

    if (moves.empty())
    {
        result.score = evaluate(board);
        result.bestMove = Move{};
        return result;
    }

    int alpha = -INF;
    int beta = INF;
    int bestScore = -INF;
    Move bestMove = moves.front();

    for (auto &m : moves)
    {
        MoveState st;
        MoveGen::makeMove(board, m, st);

        Move dummy;
        int score = -negamax(board, depth - 1, -beta, -alpha, result.nodes, dummy);

        MoveGen::unmakeMove(board, m, st);

        if (score > bestScore)
        {
            bestScore = score;
            bestMove = m;
        }

        if (score > alpha)
            alpha = score;
    }

    result.bestMove = bestMove;
    result.score = bestScore;
    return result;
}

int Search::negamax(Board &board, int depth, int alpha, int beta, uint64_t &nodes, Move &bestMoveOut)
{
    if (depth == 0)
    {
        ++nodes;
        return evaluate(board);
    }

    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board, moves);

    if (moves.empty())
    {
        ++nodes;
        return evaluate(board);
    }

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

        if (best > alpha)
            alpha = best;
        if (alpha >= beta)
            break;
    }

    bestMoveOut = bestMove;
    return best;
}
