#pragma once
#include "board/Board.h"
#include "board/MoveGen.h"
#include "engine/Evaluation.h"
#include <cstdint>
#include <vector>

struct SearchResult
{
    Move bestMove{};
    int score = 0;
    uint64_t nodes = 0;
};

class Search
{
public:
    static SearchResult think(Board &board);
    static SearchResult think(Board &board, int maxDepth);  // Overloaded version with depth limit

private:
    static int negamax(Board &board, int depth, int alpha, int beta, uint64_t &nodes, Move &bestMoveOut, int ply);
    static int quiescence(Board &board, int alpha, int beta, uint64_t &nodes);
};
