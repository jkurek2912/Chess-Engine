#include "Search.h"
#include "MoveGen.h"
#include "Transposition.h"
#include "Evaluation.h"
#include <algorithm>
#include <cassert>
#include <climits>

static constexpr int MATE_SCORE = 1000000;
static constexpr int INF = MATE_SCORE + 10000;
static constexpr int MAX_PLY = 127;
static constexpr int MAX_KILLER_PLY = 127;

static constexpr int TT_MOVE_SCORE = 1000000;
static constexpr int CAPTURE_SCORE_BASE = 100000;
static constexpr int PROMOTION_SCORE = 90000;
static constexpr int KILLER_MOVE_SCORE = 80000;

inline int mvvLvaScore(const Board &board, const Move &m)
{
    static const int pieceValue[6] = {100, 300, 325, 500, 900, 10000};
    // captured piece value – 0.1 × attacker value
    Piece capturedPiece = board.findPiece(m.to).first;
    return 10 * pieceValue[capturedPiece] - pieceValue[m.piece] / 10;
}

static TranspositionTable TT;

static Move killerMoves[MAX_KILLER_PLY + 1][2];
static int historyTable[6][64];
static constexpr int HISTORY_MAX = INT_MAX / 2; // Prevent overflow

static int materialCount(const Board &board)
{
    uint64_t total = 0;

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

    return static_cast<int>(total > 64 ? 64 : total);
}

static constexpr int DEPTH_FULL_BOARD = 6;
static constexpr int DEPTH_MIDGAME = 8;
static constexpr int MATERIAL_THRESHOLD_FULL = 26;
static constexpr int MATERIAL_THRESHOLD_MID = 10;

static int dynamicDepth(const Board &board)
{
    int pieces = materialCount(board);

    if (pieces >= MATERIAL_THRESHOLD_FULL)
        return DEPTH_FULL_BOARD;
    else if (pieces >= MATERIAL_THRESHOLD_MID)
        return DEPTH_MIDGAME;
    else
        return DEPTH_MIDGAME;
}

void orderMoves(const Board &board, std::vector<Move> &moves, Move ttBestMove, int ply)
{
    assert(ply >= 0 && ply <= MAX_KILLER_PLY);

    std::vector<std::pair<int, Move>> scored;
    scored.reserve(moves.size());

    for (auto &m : moves)
    {
        int score = 0;

        if (m.from == ttBestMove.from && m.to == ttBestMove.to)
        {
            score = TT_MOVE_SCORE;
        }
        else if (m.isCapture)
        {
            score = CAPTURE_SCORE_BASE + mvvLvaScore(board, m);
        }
        else if (m.isPromotion)
        {
            score = PROMOTION_SCORE;
        }
        else if ((ply <= MAX_KILLER_PLY) &&
                 ((m.from == killerMoves[ply][0].from && m.to == killerMoves[ply][0].to) ||
                  (m.from == killerMoves[ply][1].from && m.to == killerMoves[ply][1].to)))
        {
            score = KILLER_MOVE_SCORE;
        }
        else
        {
            score = historyTable[m.piece][m.to];
        }
        scored.emplace_back(score, m);
    }

    std::stable_sort(scored.begin(), scored.end(),
                     [](const auto &a, const auto &b)
                     { return a.first > b.first; });

    for (size_t i = 0; i < moves.size(); ++i)
        moves[i] = scored[i].second;
}

SearchResult Search::think(Board &board)
{
    if (__builtin_popcountll(board.kings[WHITE]) != 1 ||
        __builtin_popcountll(board.kings[BLACK]) != 1)
    {
        SearchResult result{};
        result.score = 0;
        result.nodes = 0;
        return result;
    }

    for (int i = 0; i <= MAX_KILLER_PLY; ++i)
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
        result.bestMove = Move{};
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
    assert(ply >= 0 && ply <= MAX_PLY);
    nodes++;

    if (board.isDraw())
    {
        int eval = evaluate(board);

        // Apply penalty/bonus based on evaluation
        // When winning, heavily penalize to avoid repetition
        // When losing, apply smaller penalty (repetitions acceptable when losing)
        // When drawn, return 0
        if (eval > 0)
        {
            return eval - 50;
        }
        else if (eval < 0)
        {
            return eval + 10;
        }
        else
        {
            return 0;
        }
    }

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
            if (!m.isCapture && !m.isPromotion && ply <= MAX_KILLER_PLY)
            {
                if (killerMoves[ply][0].from != m.from || killerMoves[ply][0].to != m.to)
                {
                    killerMoves[ply][1] = killerMoves[ply][0];
                    killerMoves[ply][0] = m;
                }
                int increment = depth * depth;
                if (historyTable[m.piece][m.to] < HISTORY_MAX - increment)
                {
                    historyTable[m.piece][m.to] += increment;
                }
                else
                {
                    for (int i = 0; i < 6; ++i)
                        for (int j = 0; j < 64; ++j)
                            historyTable[i][j] /= 2;
                    historyTable[m.piece][m.to] = increment;
                }
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
