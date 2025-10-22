#include "Search.h"
#include "MoveGen.h"
#include "Transposition.h"
#include <algorithm>
#include <future>
#include <limits>
#include <iostream>

static constexpr int MATE_SCORE = 1000000;
static constexpr int INF = MATE_SCORE + 10000;

inline int mvvLvaScore(Board &board, const Move &m)
{
    static const int pieceValue[6] = {100, 300, 325, 500, 900, 10000};
    // captured piece value – 0.1 × attacker value
    Piece capturedPiece = board.findPiece(m.to).first;
    return 10 * pieceValue[capturedPiece] - pieceValue[m.piece] / 10;
}

static TranspositionTable TT;

int materialCount(Board &board)
{
    int total = 0;

    total += __builtin_popcountll(board.pawns[WHITE]);
    total += __builtin_popcountll(board.knights[WHITE]);
    total += __builtin_popcountll(board.bishops[WHITE]);
    total += __builtin_popcountll(board.rooks[WHITE]);
    total += __builtin_popcountll(board.queens[WHITE]);
    total += __builtin_popcountll(board.kings[WHITE]);

    total += __builtin_popcountll(board.pawns[BLACK]);
    total += __builtin_popcountll(board.knights[BLACK]);
    total += __builtin_popcountll(board.bishops[BLACK]);
    total += __builtin_popcountll(board.rooks[BLACK]);
    total += __builtin_popcountll(board.queens[BLACK]);
    total += __builtin_popcountll(board.kings[BLACK]);

    return total;
}

int dynamicDepth(Board &board)
{
    int pieces = materialCount(board);

    if (pieces >= 26)
        return 6;
    else if (pieces >= 10)
        return 10;
    else
        return 8;
}

void orderMoves(Board &board, std::vector<Move> &moves)
{
    std::sort(moves.begin(), moves.end(),
              [&board](const Move &a, const Move &b)
              {
                  if (a.isCapture != b.isCapture)
                      return a.isCapture;
                  if (a.isCapture && b.isCapture)
                      return mvvLvaScore(board, a) > mvvLvaScore(board, b);
                  return false;
              });
}

SearchResult Search::think(Board &board)
{
    int searchDepth = dynamicDepth(board);
    SearchResult result{};
    result.nodes = 0;

    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board, moves);
    orderMoves(board, moves);

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
    int alpha = -INF;

    for (auto &m : moves)
    {
        Board localBoard = board;
        MoveState st;
        MoveGen::makeMove(localBoard, m, st);

        uint64_t localNodes = 0;
        Move dummy;

        int score = -negamax(localBoard, searchDepth - 1, -INF, -alpha, localNodes, dummy, 1);

        MoveGen::unmakeMove(localBoard, m, st);
        totalNodes += localNodes;

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
    uint64_t hash = board.hash;
    TTEntry entry;

    if (TT.probe(hash, entry) && entry.depth >= depth)
    {
        switch (entry.type)
        {
        case NodeType::EXACT:
            bestMoveOut = entry.bestMove;
            return entry.score;
        case NodeType::LOWERBOUND:
            alpha = std::max(alpha, entry.score);
            break;
        case NodeType::UPPERBOUND:
            beta = std::min(beta, entry.score);
            break;
        }
        if (alpha >= beta)
            return entry.score;
    }

    int alphaOrig = alpha;
    int betaOrig = beta;

    if (moves.empty())
    {
        bool inCheck = MoveGen::inCheck(board, board.whiteToMove ? WHITE : BLACK);
        return inCheck ? -MATE_SCORE + ply : 0;
    }

    if (depth == 0)
    {
        // return evaluate(board);
        return quiescence(board, alpha, beta, nodes);
    }

    orderMoves(board, moves);

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

    NodeType type = NodeType::EXACT;
    if (bestScore <= alphaOrig)
        type = NodeType::UPPERBOUND;
    else if (bestScore >= betaOrig)
        type = NodeType::LOWERBOUND;

    TT.store(hash, depth, bestScore, type, bestMoveLocal);

    bestMoveOut = bestMoveLocal;
    return bestScore;
}
