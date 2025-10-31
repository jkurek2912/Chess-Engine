#pragma once

#include "Board.h"

#include <unordered_map>
#include <shared_mutex>
#include <cstdint>
#include <algorithm>

enum class NodeType : uint8_t
{
    EXACT,
    LOWERBOUND,
    UPPERBOUND
};

struct TTEntry
{
    uint64_t key = 0;
    int depth = -1;
    int score = 0;
    NodeType type = NodeType::EXACT;
    Move bestMove{};
};

class TranspositionTable
{
    static constexpr size_t TABLE_SIZE = 1ULL << 22; // ~4M entries (~64MB)
    std::vector<TTEntry> table;

    inline TTEntry &entry(uint64_t hash) noexcept
    {
        return table[hash & (TABLE_SIZE - 1)];
    }

public:
    TranspositionTable() : table(TABLE_SIZE) {}

    bool probe(uint64_t hash, TTEntry &out) const noexcept
    {
        const TTEntry &e = table[hash & (TABLE_SIZE - 1)];
        if (e.key == hash)
        {
            out = e;
            return true;
        }
        return false;
    }

    // Replacement strategy: always-replace if different position or if new depth >= existing depth
    // This is an aggressive replacement strategy that favors newer/deeper entries
    void store(uint64_t hash, int depth, int score, NodeType type, const Move& bestMove) noexcept
    {
        TTEntry& e = table[hash & (TABLE_SIZE - 1)];
        if (e.key != hash || depth >= e.depth)
        {
            e.key = hash;
            e.depth = depth;
            e.score = score;
            e.type = type;
            e.bestMove = bestMove;
        }
    }

    void clear() noexcept
    {
        for (auto &e : table)
            e.key = 0;
    }
};