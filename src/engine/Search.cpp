#include "Search.h"
#include "MoveGen.h"
#include <algorithm>
#include <future>
#include <limits>
#include <iostream>

static constexpr int MATE_SCORE = 1000000;
static constexpr int INF = MATE_SCORE + 10000;

static std::unordered_map<uint64_t, TTEntry> transpositionTable;

void orderMoves(std::vector<Move> &moves)
{
    std::sort(moves.begin(), moves.end(), [](const Move &a, const Move &b)
              {
                  if (a.isCapture != b.isCapture)
                      return a.isCapture;
                  if (a.from != b.from)
                      return a.from < b.from;
                  return a.to < b.to; });
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
        {
            result.score = -MATE_SCORE;
        }
        else
        {
            result.score = 0;
        }
        return result;
    }

    std::vector<std::future<std::pair<int, uint64_t>>> futures;
    futures.reserve(moves.size());

    std::atomic<int> sharedAlpha{-INF};

    for (auto &m : moves)
    {
        futures.push_back(std::async(std::launch::async, [&, m]() -> std::pair<int, uint64_t>
                                     {
            Board localBoard = board;
            MoveState st;
            MoveGen::makeMove(localBoard, m, st);

            uint64_t localNodes = 0;
            Move dummy;

            int localAlpha = sharedAlpha.load(std::memory_order_relaxed);

            int score = -negamax(localBoard, depth - 1, -INF, -localAlpha, localNodes, dummy, 1);

            MoveGen::unmakeMove(localBoard, m, st);

            int oldAlpha = sharedAlpha.load();
            while (score > oldAlpha && !sharedAlpha.compare_exchange_weak(oldAlpha, score))
                ;

            return {score, localNodes}; }));
    }

    int bestScore = -INF;
    Move bestMove{};
    uint64_t totalNodes = 0;

    for (size_t i = 0; i < moves.size(); ++i)
    {
        auto [score, nodes] = futures[i].get();
        totalNodes += nodes;

        if (score > bestScore)
        {
            bestScore = score;
            bestMove = moves[i];
        }
    }

    result.bestMove = bestMove;
    result.score = bestScore;
    result.nodes = totalNodes;
    return result;
}

int Search::quiescence(Board &board, int alpha, int beta, uint64_t &nodes)
{
    nodes++;
    int standPat = evaluate(board);

    if (standPat >= beta)
        return beta;
    if (standPat > alpha)
        alpha = standPat;

    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board, moves);

    // Only consider captures
    for (auto &m : moves)
        if (m.isCapture)
        {
            MoveState st;
            MoveGen::makeMove(board, m, st);
            int score = -quiescence(board, -beta, -alpha, nodes);
            MoveGen::unmakeMove(board, m, st);

            if (score >= beta)
                return beta;
            if (score > alpha)
                alpha = score;
        }

    return alpha;
}

int Search::negamax(Board &board, int depth, int alpha, int beta,
                    uint64_t &nodes, Move &bestMoveOut, int ply)
{
    nodes++;

    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board, moves);

    if (moves.empty())
    {
        bool inCheck = MoveGen::inCheck(board, board.whiteToMove ? WHITE : BLACK);
        return inCheck ? -MATE_SCORE + ply : 0;
    }

    if (depth == 0)
        return evaluate(board);

    orderMoves(moves);

    int bestScore = -INF;
    Move bestMoveLocal{};
    MoveState state;

    for (auto &m : moves)
    {
        MoveGen::makeMove(board, m, state);
        Move childBest{};
        int score = -negamax(board, depth - 1, -beta, -alpha, nodes, childBest, ply + 1);
        MoveGen::unmakeMove(board, m, state);

        if (score > bestScore)
        {
            bestScore = score;
            bestMoveLocal = m;
        }

        if (bestScore > alpha)
            alpha = bestScore;

        if (alpha >= beta)
        {
            break;
        }
    }

    bestMoveOut = bestMoveLocal;
    return bestScore;
}
