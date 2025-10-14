#pragma once

#include "Board.h"
#include "Search.h"

#include <unordered_map>
#include <shared_mutex>
#include <cstdint>
#include <mutex>

enum class NodeType : uint8_t
{
    EXACT,
    LOWERBOUND,
    UPPERBOUND
};

struct TTEntry
{
    uint64_t zobrist;
    int depth;
    int score;
    NodeType type;
    Move bestMove;
};
class TranspositionTable
{
    std::unordered_map<uint64_t, TTEntry> table;
    mutable std::shared_mutex mutex;

public:
    bool probe(uint64_t hash, TTEntry &entry) const
    {
        std::shared_lock lock(mutex);
        auto it = table.find(hash);
        if (it == table.end())
            return false;
        entry = it->second;
        return true;
    }

    void store(uint64_t hash, int depth, int score, NodeType type, const Move &bestMove)
    {
        std::unique_lock lock(mutex);
        auto it = table.find(hash);
        if (it == table.end() || depth >= it->second.depth)
        {
            table[hash] = TTEntry{hash, depth, score, type, bestMove};
        }
    }

    void clear()
    {
        std::unique_lock lock(mutex);
        table.clear();
    }
};
