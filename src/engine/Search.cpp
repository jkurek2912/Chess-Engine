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

static Move killerMoves[128][2]; // Assuming max depth 64, with safety margin

static int historyTable[6][64];

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
        return 8;
    else
        return 8;
}

void orderMoves(Board &board, std::vector<Move> &moves, Move ttBestMove, int ply)
{
    std::vector<std::pair<int, Move>> scored;
    scored.reserve(moves.size());

    for (auto &m : moves)
    {
        int score = 0;

        if (m.from == ttBestMove.from && m.to == ttBestMove.to)
        {
            score = 1000000;
        }
        else if (m.isCapture)
        {
            score = 100000 + mvvLvaScore(board, m);
        }
        else if (m.isPromotion)
        {
            score = 90000;
        }
        else if ((m.from == killerMoves[ply][0].from && m.to == killerMoves[ply][0].to) ||
                 (m.from == killerMoves[ply][1].from && m.to == killerMoves[ply][1].to))
        {
            score = 80000;
        }
        else
        {
            score = historyTable[m.piece][m.to];
        }
        scored.emplace_back(score, m);
    }

    std::stable_sort(scored.begin(), scored.end(),
                     [](auto &a, auto &b)
                     { return a.first > b.first; });

    for (size_t i = 0; i < moves.size(); ++i)
        moves[i] = scored[i].second;
}

SearchResult Search::think(Board &board)
{
    for (int i = 0; i < 128; ++i)
    {
        killerMoves[i][0] = Move{};
        killerMoves[i][1] = Move{};
    }
    for (int i = 0; i < 6; ++i)
        for (int j = 0; j < 64; ++j)
            historyTable[i][j] = 0;

    int searchDepth = dynamicDepth(board);
    SearchResult result{};
    result.nodes = 0;

    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board, moves);

    Move ttBestMove{};
    uint64_t hash = board.hash;
    TTEntry entry;
    if (TT.probe(hash, entry))
        ttBestMove = entry.bestMove;

    orderMoves(board, moves, ttBestMove, 0);

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
    int beta = INF;

    for (auto &m : moves)
    {
        MoveState st;
        MoveGen::makeMove(board, m, st);

        uint64_t localNodes = 0;
        Move dummy;

        int score = -negamax(board, searchDepth - 1, -beta, -alpha, localNodes, dummy, 1);

        MoveGen::unmakeMove(board, m, st);
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

    // Order captures by MVV-LVA
    std::vector<std::pair<int, Move>> captureMoves;
    for (auto &m : moves)
    {
        if (m.isCapture)
        {
            int score = mvvLvaScore(board, m);
            captureMoves.emplace_back(score, m);
        }
        else if (m.isPromotion)
        {
            captureMoves.emplace_back(50000, m);
        }
    }

    std::sort(captureMoves.begin(), captureMoves.end(),
              [](auto &a, auto &b)
              { return a.first > b.first; });

    // Consider captures and promotions
    for (auto &[score, m] : captureMoves)
    {
        MoveState st;
        MoveGen::makeMove(board, m, st);
        int evalScore = -quiescence(board, -beta, -alpha, nodes);
        MoveGen::unmakeMove(board, m, st);

        if (evalScore >= beta)
            return beta;
        if (evalScore > alpha)
            alpha = evalScore;
    }

    return alpha;
}

int Search::negamax(Board &board, int depth, int alpha, int beta,
                    uint64_t &nodes, Move &bestMoveOut, int ply)
{
    nodes++;

    // Check for repetitions/draws
    if (board.isDraw())
        return 0;

    uint64_t hash = board.hash;
    TTEntry entry;
    Move ttBestMove{};

    if (TT.probe(hash, entry))
    {
        ttBestMove = entry.bestMove;
        if (entry.depth >= depth)
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
    }

    int alphaOrig = alpha;
    int betaOrig = beta;

    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board, moves);

    if (moves.empty())
    {
        bool inCheck = MoveGen::inCheck(board, board.whiteToMove ? WHITE : BLACK);
        return inCheck ? -MATE_SCORE + ply : 0;
    }

    if (depth == 0)
    {
        return quiescence(board, alpha, beta, nodes);
    }

    orderMoves(board, moves, ttBestMove, ply);

    int bestScore = -INF;
    Move bestMoveLocal{};
    MoveState state;
    int movesSearched = 0;

    for (auto &m : moves)
    {
        MoveGen::makeMove(board, m, state);
        Move childBest{};
        int score;
        movesSearched++;

        bool reduce = false;
        int reduction = 0;
        if (depth >= 3 && movesSearched > 4 && !m.isCapture && !m.isPromotion && !MoveGen::inCheck(board, board.whiteToMove ? WHITE : BLACK))
        {
            reduce = true;
            reduction = 1 + (movesSearched > 8 ? 1 : 0);
        }

        if (reduce)
        {
            score = -negamax(board, depth - 1 - reduction, -alpha - 1, -alpha, nodes, childBest, ply + 1);
            if (score > alpha)
            {
                score = -negamax(board, depth - 1, -beta, -alpha, nodes, childBest, ply + 1);
            }
        }
        else
        {
            score = -negamax(board, depth - 1, -beta, -alpha, nodes, childBest, ply + 1);
        }

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
            if (!m.isCapture && !m.isPromotion)
            {
                if (killerMoves[ply][0].from != m.from || killerMoves[ply][0].to != m.to)
                {
                    killerMoves[ply][1] = killerMoves[ply][0];
                    killerMoves[ply][0] = m;
                }
                historyTable[m.piece][m.to] += depth * depth;
            }
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
